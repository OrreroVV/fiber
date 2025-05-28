#ifndef __UTILS_H__
#define __UTILS_H__

/***
 * @Author: Orrero
 * @Date: 2024-12-25
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved.
 */
#include "log.h"
#include <execinfo.h>
#include <sys/time.h>
#include <unistd.h>
#include <syscall.h>

#if defined(__DEBUG__)
    #define LOG_INFO(format, ...) Logger::getInstance().logf(Logger::INFO, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
    #define LOG_DEBUG(format, ...) Logger::getInstance().logf(Logger::DEBUG, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
    #define LOG_ERROR(format, ...) Logger::getInstance().logf(Logger::ERROR, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#else
    #define LOG_INFO(format, ...) 
    #define LOG_DEBUG(format, ...)
    #define LOG_ERROR(format, ...)
#endif

namespace hzh {


inline pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

uint64_t GetCurrentMS();

uint64_t GetCurrentUS();

template<class T>
T& getInstance() {

}

}

#endif // __UTILS_H__