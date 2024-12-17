#include "log.h"

#include <execinfo.h>
#include <sys/time.h>
#include <unistd.h>
#include <syscall.h>

#define LOG_INFO(format, ...) Logger::getInstance().logf(Logger::INFO, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) Logger::getInstance().logf(Logger::DEBUG, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Logger::getInstance().logf(Logger::ERROR, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)

namespace hzh {


pid_t GetThreadId() {
    return syscall(SYS_gettid);
}


}