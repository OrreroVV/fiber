/*** 
 * @Author: Orrero
 * @Date: 2024-12-25
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved. 
 */
#include "hzh/scheduler.h"
#include "hzh/utils.h"

void func() {
    LOG_INFO("func start");
}

int main() {

    hzh::Scheduler sc(3, true, "test");
    sc.start();
    sleep(2);
    sc.schedule(func);
    sc.stop();
    return 0;
}