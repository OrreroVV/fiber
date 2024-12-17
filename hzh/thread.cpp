/*** 
 * @Author: Orrero
 * @Date: 2024-12-13
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved. 
 */


#include "thread.h"
#include "utils.h"

namespace hzh{

Semaphore::Semaphore(uint32_t count) {
    if (sem_init(&m_semaphore, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}
Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}
void Semaphore::wait() {
    if (sem_wait(&m_semaphore)) {
        throw std::logic_error("sem_wait error");
    }
}
void Semaphore::notify() {
    if (sem_post(&m_semaphore)) {
        throw std::logic_error("sem_post error");
    }
}

Thread::Thread(std::function<void()> cb, const std::string& name)
    :m_cb(cb),
    m_name(name) {
    if (m_name.empty()) {
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) {
        LOG_INFO("pthread_create thread fail, rt=%d", rt);
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait();

}

Thread::~Thread() {
    if (m_thread) {
        pthread_detach(m_thread);
    }
}

void Thread::join() {
    if (m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if (rt) {
            LOG_INFO("pthread_join thread fail, rt=%d, name=%s", rt, m_name.c_str());
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

void* Thread::run(void* arg) {
    Thread* thread = (Thread*)arg;

    thread->m_id = hzh::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());


    std::function<void()>cb;
    swap(cb, thread->m_cb);
    LOG_INFO("t_id: %d", thread->m_id);

    thread->m_semaphore.notify();

    cb();
    return 0;
}
    
}
