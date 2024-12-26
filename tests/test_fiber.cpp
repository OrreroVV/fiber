/*** 
 * @Author: Orrero
 * @Date: 2024-12-17
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved. 
 */
#include <ucontext.h>
#include <iostream>
#include <cstring>

ucontext_t ctx1, ctx2;

void function2() {
    std::cout << "In function2: Start execution" << std::endl;
    // swapcontext(&ctx2, &ctx1); // 保存 ctx2，切换回 ctx1
    // std::cout << "In function2: After returning from swapcontext" << std::endl;
}

int main() {
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

    return 0;
}
