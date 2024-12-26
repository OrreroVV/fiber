#ifndef __TIMER_H__
#define __TIMER_H__

/***
 * @Author: Orrero
 * @Date: 2024-12-26
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved.
 */

#include <memory>
#include <vector>
#include <set>
#include "thread.h"

namespace hzh {

class TimerManager;
class Timer : public std::enable_shared_from_this<Timer> {
friend class TimerManager;

public:
    typedef std::shared_ptr<Timer> ptr;

    bool cancel();
    bool refresh();
    
    bool reset(uint64_t ms, bool from_now);

private:
    Timer(uint64_t next);
    Timer(uint64_t ms, std::function<void()>cb, bool recurring, TimerManager* manager);

    struct Comparator {
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };

private:
    /// @brief 是否为循环定时器
    bool m_recurring = false;
    /// @brief 执行时间(ms)  
    uint64_t m_ms = 0;
    /// @brief 执行精确时间
    uint64_t m_next = 0;
    /// @brief 回调函数
    std::function<void()> m_cb;

    TimerManager* m_manager = nullptr;
};


class TimerManager {
friend class Timer;

public:
    typedef RWMutex RWMutexType;

    TimerManager();
    virtual ~TimerManager();
    /**
     * @brief 添加定时器
     * 
     * @param ms 执行时间
     * @param cb 回调函数
     * @param recurring 是否循环
     * @return Timer::ptr 
     */
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb
                        ,bool recurring = false);

    /**
     * @brief 添加条件定时器
     * 
     * @param ms 
     * @param cb 
     * @param weak_cond 
     * @param recurring 
     * @return Timer::ptr 
     */
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb
                                ,std::weak_ptr<void> weak_cond
                                ,bool recurring = false);

    /**
     * @brief Get the Next Timer object
     * 
     * @return uint64_t 
     */
    uint64_t getNextTimer();

    /**
     * @brief 获取需要执行的定时器的回调函数列表
     * 
     * @param cbs 
     */
    void listExpiredCb(std::vector<std::function<void()>>& cbs);

    /**
     * @brief 是否有定时器
     * 
     * @return true 
     * @return false 
     */
    bool hasTimer();

protected:

    /**
     * @brief 当有新的定时器插入到首部，执行该函数
     * 
     */
    virtual void onTimerInsertedAtFront() = 0;


    /**
     * @brief 将定时器添加到管理器中 
     * 
     * @param val 
     * @param lock 
     */
    void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);

    /**
     * @brief 检测服务器时间是否被调后
     * 
     * @param now_ms 
     * @return true 
     * @return false 
     */
    bool detectClockRollover(uint64_t now_ms);


private:
    RWMutexType m_mutex;
    /// @brief 定时器集合
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    /// @brief 是否触发onTimerInsertedAtFront
    bool m_tickled = false;
    /// @brief 上次执行时间
    uint64_t m_previousTime = 0;

};

}

#endif // __TIMER_H__