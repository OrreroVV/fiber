/*** 
 * @Author: Orrero
 * @Date: 2024-12-17
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved. 
 */

 #include "hzh/utils.h"
 #include "hzh/singleton.h"
 #include "hzh/iomanager.h"
 #include <functional>
 #include <vector>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <iostream>
 #include <sys/epoll.h>
 #include <cstring>
 #include <cassert>

ucontext_t ctx1, ctx2;

void function2() {
    std::cout << "In function2: Start execution" << std::endl;
    // swapcontext(&ctx2, &ctx1); // 保存 ctx2，切换回 ctx1
    // std::cout << "In function2: After returning from swapcontext" << std::endl;
}


void test1() {
    char stack[8192]; // 为 function2 提供栈空间

    // 获取当前上下文，保存到 ctx2
    getcontext(&ctx2);
    ctx2.uc_link = nullptr; // 没有后继上下文
    ctx2.uc_stack.ss_sp = stack;
    ctx2.uc_stack.ss_size = sizeof(stack);

    // 设置上下文，将执行 function2
    makecontext(&ctx2, function2, 0);

    std::cout << "In main: Before swapcontext" << std::endl;

    // 切换上下文：保存当前上下文到 ctx1，切换到 ctx2 执行 function2
    swapcontext(&ctx1, &ctx2);

    std::cout << "In main: After returning from swapcontext" << std::endl;
}


void run_in_fiber() {
    hzh::Fiber::ptr cur = hzh::Fiber::GetThis();
    LOG_INFO("run_in_fiber begin");
    // hzh::Fiber::YieldToHold();
    cur->back();
    LOG_INFO("run_in_fiber end");
    cur->back();
    // hzh::Fiber::YieldToHold();
}

void test_fiber() {
    {
        hzh::Fiber::GetThis();
        LOG_INFO("main begin");
        hzh::Fiber::ptr fiber(new hzh::Fiber(run_in_fiber));
        fiber->call();
        LOG_INFO("main after swapIn");
        fiber->call();
        LOG_INFO("main after end");
        fiber->call();
    }
    LOG_INFO("main after end 2");
}

void tset2() {
    hzh::Thread::SetName("main");

    std::vector<hzh::Thread::ptr> thrs;
    for(int i = 0; i < 1; ++i) {
        thrs.push_back(hzh::Thread::ptr(
                    new hzh::Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for(auto i : thrs) {
        i->join();
    }
}


int main() {
    // test1();
    tset2();
    return 0;
}
