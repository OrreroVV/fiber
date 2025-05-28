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

    }

    ~ThreadTool() {
        stop();
    }

    template<typename F, typename... Args>
    void enqueue(F&& f, Args... args) {
        // using result_type = typename std::result_of_t<F(Args...)>::type;

        std::tuple<Args...> tuple_args = std::make_tuple(std::forward<Args>(args)...);
        std::function<void()> func = [f = std::forward<F>(f), tuple_args]() {
            std::apply(f, tuple_args);
        };
        std::function<void()> task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        {
            std::unique_lock<std::mutex>(m_queueMutex);
            m_tasks.emplace([task]()->void{ task();});
        }
        m_condition.notify_one();
    }

    void start(size_t numThreads = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < numThreads; ++i) {
            m_threads.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(m_queueMutex);
                        m_condition.wait(lock, [this]() {
                            return m_stopped || !m_tasks.empty();
                        });

                        if (m_stopped && m_tasks.empty())
                            return;

                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    void stop() {
        {
            m_stopped = true;
        }
        m_condition.notify_all();

        for (std::thread &t : m_threads) {
            if (t.joinable())
                t.join();
        }

        m_threads.clear();
    }

private:
    std::atomic<bool> m_stopped = { false };
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
};













#endif // __THREADTOOL_H__