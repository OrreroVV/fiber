#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

/***
 * @Author: Orrero
 * @Date: 2024-12-17
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved.
 */

#include <memory>
#include <vector>
#include <list>
#include "thread.h"
#include "fiber.h"
#include "utils.h"

namespace hzh {


class Scheduler {

public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name ="");
    virtual ~Scheduler();

    const std::string& getName() const { return m_name; }
    static Scheduler* GetThis();
    static Fiber* GetMainFiber();
    void start();
    void stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {
        {
            MutexType::Lock lock(m_mutex);
            scheduleNoLock(fc, thread);
        }
    }

    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end) {
        {
            MutexType::Lock lock(m_mutex);
            while (begin != end) {
                scheduleNoLock(*begin, -1);
                ++begin;
            }
        }
    }

protected:
    virtual void tickle();

    void run();

    virtual bool stopping();
    virtual void idle();
    void setThis();
    bool hasIdleThreads() { return m_idleThreadCount > 0; }

private:
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        
        FiberAndThread ft(fc, thread);
        if (ft.fiber || ft.cb) {
            m_tasks.push_back(ft);
        }
        return true;
    }   

private:
    struct FiberAndThread {
        Fiber::ptr fiber;
        std::function<void()>cb;
        int thread;
        FiberAndThread(Fiber::ptr f, int thr)
            :fiber(f), thread(thr) {
        }
        FiberAndThread(Fiber::ptr* f, int thr)
            :thread(thr) {
            fiber.swap(*f);
        }

        FiberAndThread(std::function<void()> f, int thr)
            :cb(f), thread(thr) {

        }
        FiberAndThread(std::function<void()>* f, int thr)
            :thread(thr) {
            cb.swap(*f);
        }
        
        FiberAndThread() 
            :thread(-1){

        }

        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
        
    };


private:
    MutexType m_mutex;
    std::vector<Thread::ptr> m_threads;
    std::list<FiberAndThread> m_tasks;
    // use_caller = true时有效，调度协程
    Fiber::ptr m_rootFiber;
    std::string m_name;
protected:
    std::vector<int> m_threadIds;
    size_t m_threadCount = 0;
    std::atomic<size_t> m_activeThreadCount = { 0 };
    std::atomic<size_t> m_idleThreadCount = { 0 };
    bool m_stopping = true;
    bool m_autoStop = false;
    int m_rootThread = 0;
};


}

#endif // __SCHEDULER_H__