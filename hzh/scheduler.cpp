/*** 
 * @Author: Orrero
 * @Date: 2024-12-17
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved. 
 */

#include "scheduler.h"
#include "utils.h"
#include <assert.h>


namespace hzh {

// 调度器
static thread_local Scheduler* t_scheduler = nullptr;
// 调度协程
static thread_local Fiber* t_scheduler_fiber = nullptr;

/**
 * 使用use_caller使用调度
 * 一个协程(m_rootFiber)作为主线程
 * 创建其他threads - 1个线程
 * 
 * 创建scheduler的线程：称之为主线程
 * scheduler可以选择创建threads个线程
 * 每个线程都有一个独立的t_scheduler_fiber和t_scheduler
 * 对于创建的threads个线程来说 每个创建的线程都有一个主协程（调度器），(Fiber默认构造函数创建的)
 * 并且赋值给t_scheduler_fiber(t_scheduler_fiber = Fiber::GetThis())
 * 当fiber塞入到schedule(此时fiber的状态都是就绪状态，不属于任何一个协程调度)
 * scheduler会进行分配，分配到某个线程，就归该线程所属的主协程管理
 * 
 * 
 * 
 */
Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) 
    :m_name(name){
    assert(threads > 0);
    
    if (use_caller) {
        hzh::Fiber::GetThis();

        --threads;
        assert(GetThis() == nullptr);
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        hzh::Thread::SetName(name);

        t_scheduler_fiber = m_rootFiber.get();
        m_rootThread = hzh::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    } else {
        m_rootThread = -1;
    }
    m_threadCount = threads;
}

Scheduler::~Scheduler() {
    assert(m_stopping);
    if(GetThis() == this) {
        t_scheduler = nullptr;
    }
}

Scheduler* Scheduler::GetThis() {
    return t_scheduler;
}

Fiber* Scheduler::GetMainFiber() {
    return t_scheduler_fiber;
}

void Scheduler::start() {
    MutexType::Lock lock(m_mutex);
    if (!m_stopping) {
        return;
    }

    m_stopping = false;
    assert(m_threads.empty());

    m_threads.resize(m_threadCount);
    for (int i = 0; i < m_threadCount; ++i) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this)
                            , m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
    lock.unlock();
}

void Scheduler::stop() {
    m_autoStop = true;
    if(m_rootFiber
            && m_threadCount == 0
            && (m_rootFiber->getState() == Fiber::TERM
                || m_rootFiber->getState() == Fiber::INIT)) {
        m_stopping = true;

        if(stopping()) {
            return;
        }
    }

    //bool exit_on_this_fiber = false;
    if(m_rootThread != -1) {
        assert(GetThis() == this);
    } else {
        assert(GetThis() != this);
    }

    m_stopping = true;
    for(size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }

    if(m_rootFiber) {
        tickle();
    }

    if(m_rootFiber) {
        //while(!stopping()) {
        //    if(m_rootFiber->getState() == Fiber::TERM
        //            || m_rootFiber->getState() == Fiber::EXCEPT) {
        //        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        //        SYLAR_LOG_INFO(g_logger) << " root fiber is term, reset";
        //        t_fiber = m_rootFiber.get();
        //    }
        //    m_rootFiber->call();
        //}
        if(!stopping()) {
            m_rootFiber->call();
        }
    }

    std::vector<Thread::ptr> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }

    for(auto& i : thrs) {
        i->join();
    }
}

void Scheduler::tickle() {
    LOG_DEBUG("tickle");
}

void Scheduler::run() {
    // 设置当前运行的scheduler，每个线程的t_scheduler都指向自己
    setThis();
    /**
     * 如果不是主线程调度的话，则t_scheduler_fiber并不是有效的（指向的还是主线程的t_scheduler_fiber），需要把该线程的调度协程赋值给t_scheduler_fiber
     * case: GetThis() --> 如果Fiber::t_scheduler_fiber为空的话，则会创建一个新的协程
     * 
     * case: 主协程是m_rootFiber，这个协程是user_caller = true时所在的线程，也就是主线程
     * 如果new Thread创建的线程运行run函数时，当前线程所对应的local thread : t_scheduler_fiber一定是nullptr，需要重新为该线程赋值一个调度协程
     */
    if (hzh::GetThreadId() != m_rootThread) {
        t_scheduler_fiber = Fiber::GetThis().get();
    }
    LOG_INFO("scheduler run fiber id %d, %d", t_scheduler_fiber->m_id, GetThreadId());

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));

    // 回调函数
    Fiber::ptr cb_fiber;

    // 找到所要执行的第一个可用任务
    FiberAndThread ft;

    while (true) {
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            MutexType::Lock lock(m_mutex);
            auto it = m_tasks.begin();
            while (it != m_tasks.end()) {
                // 如果该任务指定线程并且并不是该线程，需要触发tickle唤醒协程
                if (it->thread != -1 && it->thread != hzh::GetThreadId()) {
                    ++it;
                    tickle_me = true;
                    continue;
                }

                assert(it->fiber || it->cb);
                if (it->fiber && it->fiber->getState() == Fiber::EXEC) {
                    ++it;
                    continue;
                }

                ft = *it;
                m_tasks.erase(it);
                ++m_activeThreadCount;
                is_active = true;
                break;
            }

            tickle_me |= it != m_tasks.end();
        }

        if (tickle_me) {
            tickle();
        }

        if (ft.fiber && (ft.fiber->getState() != Fiber::TERM
                            && ft.fiber->getState() != Fiber::EXCEPT)) {
            
            // 执行该协程上下文
            ft.fiber->swapIn();
            --m_activeThreadCount;

            /*
            如果协程并没有执行完，并且主动让出执行权，则需要重新把任务加入到执行队列中
            如果并不是执行完或者异常情况，则协程主动请求阻塞，挂起协程，该协程唤醒需要用户主动操作（定时器超时或者epoll返回主动添加到执行队列中）
            */
            if (ft.fiber->getState() == Fiber::READY) {
                schedule(ft.fiber);
            } else if (ft.fiber->getState() != Fiber::TERM
                        && ft.fiber->getState() != Fiber::EXCEPT) {
                
                ft.fiber->m_state = Fiber::HOLD;
            }
            ft.reset();
        } else if (ft.cb) {// 如果需要执行回调函数的话，创建fiber，执行fiber
            if (cb_fiber) {
                cb_fiber->reset(ft.cb);
            } else {
                cb_fiber.reset(new Fiber(ft.cb));
            }
            ft.reset();
            cb_fiber->swapIn();
            --m_activeThreadCount;
            if (cb_fiber->getState() == Fiber::READY) {
                schedule(cb_fiber);
                cb_fiber.reset();
            } else if (cb_fiber->getState() == Fiber::EXCEPT 
                        || cb_fiber->getState() == Fiber::TERM) {
                cb_fiber->reset(nullptr);
            } else {
                cb_fiber->m_state = Fiber::HOLD;
                cb_fiber.reset();
            }
        } else {
            if (is_active) {
                --m_activeThreadCount;
                continue;
            }

            if (idle_fiber->getState() == Fiber::TERM) {
                LOG_INFO("idle fiber term");
                break;
            }

            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;

            if (idle_fiber->getState() != Fiber::TERM
                && idle_fiber->getState() != Fiber::EXCEPT) {
                idle_fiber->m_state = Fiber::HOLD;
            }
        }
    }
}

void Scheduler::setThis() {
    t_scheduler = this;
}

bool Scheduler::stopping() {
    MutexType::Lock lock(m_mutex);
    return m_autoStop && m_stopping
        && m_tasks.empty() && m_activeThreadCount == 0;
}

void Scheduler::idle() {
    LOG_DEBUG("idle");
    while (!stopping()) {
        hzh::Fiber::YieldToHold();
    }
}


}

