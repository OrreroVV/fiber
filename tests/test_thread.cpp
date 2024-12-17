/*** 
 * @Author: Orrero
 * @Date: 2024-12-13
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved. 
 */
#include "hzh/thread.h"
#include "hzh/utils.h"
#include <vector>
#include <assert.h>

hzh::RWMutex mutex;
int count;
void func() {
    {
        hzh::RWMutex::WriteLock lock(mutex);
        count++;
    }
}

void func1() {
    {
        hzh::RWMutex::ReadLock lock(mutex);
        LOG_INFO("%d", count);
    }
}

int main() {
    std::vector<hzh::Thread::ptr> threads;
    for (int i = 0; i < 100; i++) {
        try {
            hzh::Thread::ptr t1(new hzh::Thread(func, "tx" + i));
            threads.push_back(t1);
        }
        catch(...) {
            continue;
        }
    }
    for (auto i : threads) {
        i->join();
    }
    LOG_INFO("END MAIN, %d", count);
    return 0;
}