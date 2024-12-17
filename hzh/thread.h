#ifndef __THREAD_H__
#define __THREAD_H__

/***
 * @Author: Orrero
 * @Date: 2024-12-16
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved.
 */

#include <thread>
#include <functional>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <atomic>
#include "noncopyable.h"

namespace hzh{

class Semaphore : Noncopyable{
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();
    void wait();
    void notify();
private:
    sem_t m_semaphore;
};

template<class T>
struct ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex)
        : m_mutex(mutex) {
        lock();
        m_locked = true;
    }   

    ~ScopedLockImpl() {
        unlock();
    } 

    void lock() {
        if (!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }


private: 
    T& m_mutex;
    bool m_locked;
};

template<class T>
struct ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T& mutex) 
        :m_mutex(mutex){
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.rdlock();
        }
        m_locked = true;
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
struct WriteScopedLockImpl {
public:
    WriteScopedLockImpl(T& mutex) 
        :m_mutex(mutex){
        m_mutex.wrlock();
        m_locked = true;
    }

    ~WriteScopedLockImpl() {
        unlock();
    }

    void lock() {
        if (!m_locked) {
            m_mutex.wrlock();
        }
        m_locked = true;
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

class RWMutex : Noncopyable {
public:
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;

    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }


    void rdlock() {
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock;
};

class Mutex : Noncopyable{
public:
    typedef ScopedLockImpl<Mutex> Lock;
    
    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }
    
    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
};

class Thread : Noncopyable{
public:
    typedef std::shared_ptr<Thread> ptr;
    
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();
    void join();
    static void* run(void* arg);

private:
    pid_t m_id;
    Semaphore m_semaphore;
    std::function<void()> m_cb;
    pthread_t m_thread;
    std::string m_name;
};


}

#endif // __THREAD_H__