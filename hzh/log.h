#ifndef __LOG_H__
#define __LOG_H__

/***
 * @Author: Orrero
 * @Date: 2024-12-16
 * @Description: 
 * @
 * @Copyright (c) 2024 by Orrero, All Rights Reserved.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <mutex>
#include <cstdarg> // For va_list, va_start, va_end
#include <sstream> // For dynamic string creation

class Logger {
public:
    enum LogLevel {
        INFO,
        DEBUG,
        ERROR
    };

    // 获取单例实例
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // 设置日志文件路径
    void setLogFile(const std::string& filePath) {
        std::lock_guard<std::mutex> lock(mutex_);
        logFilePath_ = filePath;
    }

    // 格式化日志输出
    void logf(LogLevel level, const char* file, int line, const char* func, const char* format, ...) {
        std::lock_guard<std::mutex> lock(mutex_);
        va_list args;
        va_start(args, format);
        std::string message = formatString(format, args);
        va_end(args);

        // 格式化日志输出
        std::string logMessage = formatLog(level, message, file, line, func);

        // 输出到控制台
        std::cout << logMessage;

        // 输出到文件（如果设置了日志文件路径）
        if (!logFilePath_.empty()) {
            std::ofstream logFile(logFilePath_, std::ios::app);
            if (logFile.is_open()) {
                logFile << logMessage;
                logFile.close();
            }
        }
    }

private:
    std::string logFilePath_;
    std::mutex mutex_;

    // 私有构造函数（单例模式）
    Logger() = default;

    // 禁止拷贝构造和赋值
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // 格式化日志内容
    std::string formatLog(LogLevel level, const std::string& message, const char* file, int line, const char* func) {
        const char* levelStr = nullptr;
        switch (level) {
            case INFO: levelStr = "INFO"; break;
            case DEBUG: levelStr = "DEBUG"; break;
            case ERROR: levelStr = "ERROR"; break;
            default: levelStr = "UNKNOWN"; break;
        }

        // 获取当前时间
        std::time_t now = std::time(nullptr);
        char timeBuffer[20];
        std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        // 格式化日志内容
        std::ostringstream oss;
        oss << "[" << timeBuffer << "]";
        oss << "[" << levelStr << "]";
        oss << "[" << file << ":" << line << "]";
        oss << "[" << func << "] ";
        oss << message << "\n";
        return oss.str();
    }

    // 动态格式化字符串
    std::string formatString(const char* format, va_list args) {
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        return std::string(buffer);
    }
};



#endif // __LOG_H__