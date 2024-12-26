/*** 
 * @Author: Orrero
 * @Date: 2024-12-26
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved. 
 */
#include "iomanager.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

namespace hzh {

IOManager::FdContext::EventContext& IOManager::FdContext::getContext(Event event) {
    switch (event)
    {
    case READ:
        return read;
    case WRITE:
        return write;
    default:
        assert(false);
    }

    throw std::invalid_argument("getcontext event error");
}

void IOManager::FdContext::resetContext(EventContext &ctx) {
    ctx.scheduler = nullptr;
    ctx.cb = nullptr;
    ctx.fiber.reset();
}

void IOManager::FdContext::triggerEvent(Event event) {
    assert(events & event);

    events = (Event)(events & ~event);

    EventContext ctx = getContext(event);
    if (ctx.cb) {
        ctx.scheduler->schedule(ctx.cb);
    } else if (ctx.fiber) {
        ctx.scheduler->schedule(ctx.fiber);
    }

    ctx.scheduler = nullptr;
    return;
}

IOManager::IOManager(size_t threads, bool use_caller, const std::string &name)
    :Scheduler(threads, use_caller, name) {
    m_epfd = epoll_create(4000);
    assert(m_epfd > 0);
    int rt = pipe(m_tickleFds);
    assert(!rt);
    epoll_event event;
    memset(&event, 0, sizeof(event));

    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];
    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    assert(!rt);
    contextResizeNoLock(32);
    start();
}

IOManager::~IOManager() {
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
        if (m_fdContexts[i]) {
            delete m_fdContexts[i];
        }
    }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb) {
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() <= fd) {
        lock.unlock();
        RWMutexType::WriteLock lock2(m_mutex);
        contextResizeNoLock(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    } else {
        fd_ctx = m_fdContexts[fd];
        lock.lock();
    }

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    
    // 先判断是否已经存在这个事件
    if (fd_ctx->events & event) {
        assert(false);
    }

    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->events | event;
    epevent.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt) {
        LOG_ERROR("addEvent fd:%d, rt:%d", fd, rt);
        return -1;
    }

    ++m_pendingEventCount;
    
    fd_ctx->events = (Event)(fd_ctx->events | event);
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    assert(!event_ctx.cb && !event_ctx.fiber && !event_ctx.scheduler);

    event_ctx.scheduler = Scheduler::GetThis();
    if (cb) {
        event_ctx.cb.swap(cb);
    } else {
        event_ctx.fiber = Fiber::GetThis();
    }
    return 0;
}

int IOManager::delEvent(int fd, Event event) {
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() <= fd) {
        return -1;
    }
    FdContext* fd_ctx = m_fdContexts[fd];

    if (!(fd_ctx->events & event)) {
        LOG_ERROR("delEvent not event");
        return -1;
    }
    lock.unlock();
    RWMutexType::WriteLock lock2(m_mutex);

    fd_ctx->events = (Event)(fd_ctx->events ^ event);
    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->events ^ event;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt) {
        LOG_ERROR("delEvent fd:%d, rt:%d", fd, rt);
        return -1;
    }

    --m_pendingEventCount;
    return 0;
}

void IOManager::tickle() {
    if (hasIdleThreads()) {
        return;
    }    
    int rt = write(m_tickleFds[1], "T", 1);
    assert(rt == 1);
}

bool IOManager::stopping() {
    uint64_t timer;
    return stopping(timer);
}

void IOManager::idle() {
    LOG_DEBUG("IO MANAGER IDLE");
}

void IOManager::onTimerInsertedAtFront() {

}

void IOManager::contextResizeNoLock(size_t size)
{
    m_fdContexts.resize(size);
    for (int i = 0; i < m_fdContexts.size(); ++i) {
        if (!m_fdContexts[i]) {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

bool IOManager::stopping(uint64_t &timeout) {
    timeout = getNextTimer();
    return !m_pendingEventCount
        && timeout == ~0ull
        && Scheduler::stopping();
}

}