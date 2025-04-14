#ifndef __THREADTOOL_H__
#define __THREADTOOL_H__

/***
 * @author: Orrero
 * @date: 2025-04-14
 * @brief: 
 * 
 * @copyright (c) 2025 by Orrero, All Rights Reserved.
 */


 #include <bits/stdc++.h>
 #include <fcntl.h>
 #include <sys/mman.h>
 #include <unistd.h>
 #include <cstring>
 #include <iostream>
 #include <utility>



class ThreadTool {

public:


    ThreadTool() {

    };

    template<typename F, typename... Args>
    void enqueue(F&& f, Args... args) {
        using result_type = typename std::result_of<F(Args...)>::type;

        std::tuple<Args...> tuple_args = std::make_tuple(std::forward<Args>(args)...);
        std::function<void()> func = [f = std::forward<F>(f), tuple_args]() {
            std::apply(f, tuple_args);
        };

        {
            std::unique_lock<std::mutex>(m_queueMutex);
            m_tasks.push(func);
        }
        m_condition.notify_one();
    }

    void start() {

    }



private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
};













#endif // __THREADTOOL_H__