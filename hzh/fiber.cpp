/*** 
 * @Author: Orrero
 * @Date: 2024-12-17
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved. 
 */

#include "fiber.h"
#include "utils.h"
#include "scheduler.h"
#include <assert.h>
#include <atomic>

namespace hzh {

static std::atomic<uint64_t> s_fiber_id { 0 };
static std::atomic<uint64_t> s_fiber_count { 0 };

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

static uint32_t g_fiber_stack_size = 1024 * 1024;

Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    // 把当前执行的上下文给当前主协程
    if (getcontext(&m_ctx)) {
        assert(true);
    }
    
    ++s_fiber_count;
    LOG_DEBUG("main fiber, thread: %d", GetThreadId());
}


/**
 * 一个调度器只有一个调度协程，其余use_caller = false
 */
Fiber::Fiber(std::function<void()> cb, size_t stack_size, bool use_caller) 
    :m_id(++s_fiber_id), m_cb(cb){
    m_state = INIT;
    ++s_fiber_count;
    m_stacksize = stack_size ? stack_size : g_fiber_stack_size;

    m_stack = malloc(m_stacksize);
    if (getcontext(&m_ctx)) {
        assert(true);
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    if (!use_caller) {
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }
}

Fiber::~Fiber() {
    --s_fiber_count;
    if(m_stack) {
        assert(m_state == TERM
                || m_state == EXCEPT
                || m_state == INIT);

        free(m_stack);
    } else {
        assert(!m_cb);
        assert(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }
}

void Fiber::reset(std::function<void()> cb) {
    assert(m_stack);
    assert(m_state == TERM
        || m_state == EXCEPT
        || m_state || INIT);
    m_cb = cb;
    if (getcontext(&m_ctx)) {
        assert(false);
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void Fiber::swapIn() {
    SetThis(this);
    assert(m_state != EXEC);
    m_state = EXEC;
    if (swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
        LOG_ERROR("swap context error");
        assert(false);
    };
}

void Fiber::swapOut() {
    SetThis(Scheduler::GetMainFiber());
    if (swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) {
        LOG_ERROR("swap context error");
        assert(false);
    };
}

void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    assert(!swapcontext(&t_threadFiber->m_ctx, &m_ctx));
}

void Fiber::back() {
    SetThis(t_threadFiber.get());
    assert(!swapcontext(&m_ctx, &t_threadFiber->m_ctx));
}

void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis() {
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    assert(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}   

void Fiber::YieldToReady() {
    auto cur = GetThis();
    assert(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    assert(cur->m_state == EXEC);
    cur->m_state = HOLD;
    cur->swapOut();
}

uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    LOG_INFO("mainfunc %p", static_cast<void*>(&*cur));
    assert(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch(...) {
        cur->m_state = EXCEPT;
        LOG_ERROR("Fiber except");
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();

    assert(false);

}

void Fiber::CallerMainFunc() {
    Fiber::ptr cur = GetThis();
    assert(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch(...) {
        cur->m_state = EXCEPT;
        LOG_ERROR("Fiber except");
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();

    assert(false);

}

uint64_t Fiber::GetMainFiberId() {
    if (t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}


}