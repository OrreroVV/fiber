#ifndef __IOMANAGER_H__
#define __IOMANAGER_H__

/**
 * @author Orrero
 * @date 2024-12-26
 * @brief 
 * 
 * @copyright Copyright (c) 2024 by Orrero, All Rights Reserved
 */

#include "timer.h"
#include "utils.h"
#include "scheduler.h"
#include "thread.h"
#include <string>

namespace hzh {

class IOManager : public Scheduler, public TimerManager{

public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;


    enum Event {
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0x4,
    };

private:
    struct FdContext {
        typedef Mutex MutexType;

        struct EventContext {
            Scheduler* scheduler = nullptr;
            Fiber::ptr fiber;
            std::function<void()>cb;
        };


        /**
         * @brief Get the Context object
         * 
         * @param event 
         * @return EventContext& 
         */
        EventContext& getContext(Event event);

        /**
         * @brief 重置ctx
         * 
         * @param ctx 
         */
        void resetContext(EventContext& ctx);

        /**
         * @brief 触发事件event
         * 
         * @param event 
         */
        void triggerEvent(Event event);

        EventContext read;
        EventContext write;
        int fd;

        Event events = NONE;
        MutexType mutex;
    };

public:

    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    
    ~IOManager();

    /**
     * @brief 添加事件
     * 
     * @param fd 
     * @param event 
     * @return int 
     */
    int addEvent(int fd, Event event, std::function<void()>cb = nullptr);

    /**
     * @brief 删除事件
     * 
     * @param fd 
     * @param event 
     * @return int 
     */
    int delEvent(int fd, Event event);

    /**
     * @brief 取消socket句柄对应的事件
     * 
     * @param fd 
     * @param event 
     * @return true 
     * @return false 
     */
    bool cancelEvent(int fd, Event event);

    /**
     * @brief 取消当前fd所有事件
     * @param fd 
     * @return true 
     * @return false 
     */
    bool cancelAll(int fd);

    /**
     * @brief Get the This object
     * 
     * @return IOManager* 
     */
    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    void onTimerInsertedAtFront() override;

    void contextResizeNoLock(size_t size);

    /**
     * @brief 判断是否任务做完，并且赋值下一个最近的时间给timeout
     * @param timeout 
     * @return true 
     * @return false 
     */
    bool stopping(uint64_t& timeout);

private:
    /// @brief epoll实例
    int m_epfd = 0;
    /// @brief tickle管道
    int m_tickleFds[2];
    /// @brief 等待执行的事件数量
    std::atomic<size_t> m_pendingEventCount = { 0 };
    /// @brief 局部锁
    RWMutexType m_mutex;
    /// @brief socket事件上下文容器
    std::vector<FdContext*> m_fdContexts;
};


}

#endif // __IOMANAGER_H__