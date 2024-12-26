/*** 
 * @Author: Orrero
 * @Date: 2024-12-26
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved. 
 */
#include "timer.h"
#include "utils.h"
#include <assert.h>

namespace hzh {

bool Timer::Comparator::operator()(const Timer::ptr& lhs
                        ,const Timer::ptr& rhs) const {
    if(!lhs && !rhs) {
        return false;
    }
    if(!lhs) {
        return true;
    }
    if(!rhs) {
        return false;
    }
    if(lhs->m_next < rhs->m_next) {
        return true;
    }
    if(rhs->m_next < lhs->m_next) {
        return false;
    }
    return lhs.get() < rhs.get();
}


Timer::Timer(uint64_t next)
    :m_next(next) {
    
}

Timer::Timer(uint64_t ms, std::function<void()>cb, bool recurring, TimerManager* manager)
    :m_recurring(recurring)
    ,m_ms(ms)
    ,m_cb(cb)
    ,m_manager(manager) {
    m_next = GetCurrentMS() + m_ms;
}

bool Timer::cancel() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (m_cb) {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        assert(it != m_manager->m_timers.end());
        m_manager->m_timers.erase(it);
        return true;
    }
    return false;
}
bool Timer::refresh() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_cb) {
        return false;
    }

    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    m_next = GetCurrentMS() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}
bool Timer::reset(uint64_t ms, bool from_now) {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_cb) {
        return false;
    }

    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    if (from_now) {
        m_next = GetCurrentMS() + ms;
    } else {
        m_next = m_next - m_ms + ms;
    }

    m_ms = ms;
    m_manager->addTimer(shared_from_this(), lock);
    return true;
}

TimerManager::TimerManager() {

}

TimerManager::~TimerManager() {
}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring) {
    Timer::ptr timer(new Timer(ms, cb, recurring, this));

    RWMutexType::WriteLock lock(m_mutex);
    addTimer(timer, lock);
    return timer;
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()>cb) {
    std::shared_ptr<void> temp = weak_cond.lock();
    if (temp) {
        cb();
    }
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring) {
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    if (m_timers.empty()) {
        return ~0ull;
    }
    return (*m_timers.begin())->m_next;
}

void TimerManager::listExpiredCb(std::vector<std::function<void()>> &cbs) {
    cbs.clear();
    uint64_t now = GetCurrentMS();
    {
        RWMutexType::ReadLock lock(m_mutex);
        if (m_timers.empty()) {
            return;
        }
    }

    RWMutexType::ReadLock lock(m_mutex);
    
    bool rollover = detectClockRollover(now);
    if (!rollover && (*m_timers.begin())->m_next < now) {
        return;
    }

    Timer::ptr timer(new Timer(now));
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(timer);

    while (it != m_timers.end() && (*it)->m_next == now) {
        ++it;
    }

    std::vector<Timer::ptr> expired;
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);

    cbs.reserve(expired.size());
    for (int i = 0; i < expired.size(); ++i) {
        cbs.push_back(expired[i]->m_cb);
        if (expired[i]->m_recurring) {
            expired[i]->m_next = expired[i]->m_ms + now;
            m_timers.insert(expired[i]);
        } else {
            expired[i] = nullptr;
        }
    }
}

bool TimerManager::hasTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    return m_timers.size();
}

void TimerManager::addTimer(Timer::ptr val, RWMutexType::WriteLock &lock) {
    //可以不用加锁，之前已经加锁了
    lock.lock();
    bool insertFront = false;
    auto it = m_timers.insert(val).first;
    if (it == m_timers.begin()) {
        insertFront = true;
    }
    lock.unlock();

    if (insertFront) {
        onTimerInsertedAtFront();
    }
}

bool TimerManager::detectClockRollover(uint64_t now_ms)
{
    bool rollover = false;
    // 如果当前时间小于上次执行时间，并且小于前一个小时时间
    if (now_ms < m_previousTime && now_ms < (m_previousTime - 60 * 60 * 1000)) {
        rollover = true;
    }
    m_previousTime = now_ms;
    return rollover;
}
}