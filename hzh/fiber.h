#ifndef __FIBER_H__
#define __FIBER_H__

/***
 * @Author: Orrero
 * @Date: 2024-12-17
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved.
 */

#include <memory>
#include <functional>
#include <ucontext.h>

namespace hzh {

class Fiber : public std::enable_shared_from_this<Fiber> {
friend class Scheduler;
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State {
        //初始化
        INIT,
        //阻塞
        HOLD,
        //执行中
        EXEC,
        //执行结束
        TERM,
        //准备
        READY,
        //异常
        EXCEPT,
    };

private:
    Fiber();

public:
    Fiber(std::function<void()> cb, size_t stack_size = 0, bool use_caller = false);

    ~Fiber();



    void reset(std::function<void()> cb);

    /*** 
     * @description: 切换到调度器的主协程执行
     * @return {*}
     */    
    void swapIn();

    void swapOut();

    void call();

    void back();

    uint64_t getId() const { return m_id; }

    uint64_t getState() const { return m_state; }

public:
    static void SetThis(Fiber* f);

    static Fiber::ptr GetThis();

    static void YieldToReady();

    /*** 
     * @description: 该Fiber标记为阻塞，并且通过适当的回调函数触发该Fiber.swapIn唤醒该协程继续执行
     * @return {*}
     */    
    static void YieldToHold();

    static uint64_t TotalFibers();

    static void MainFunc();

    static void CallerMainFunc();

    static uint64_t GetMainFiberId();

private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;
    ucontext_t m_ctx;
    void* m_stack = nullptr;
    std::function<void()> m_cb;
};

}

#endif // __FIBER_H__