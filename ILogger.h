//
// Created by Kai Lee on 2025/8/1.
//

#pragma once

#include <string>
#include <memory>

// 级别
enum class LogLevel {
    trace,
    debug,
    info,
    warn,
    error,
    critical
};

/**
 * @brief ILogger 日志对象类接口
 */
class ILogger {
public:
    virtual ~ILogger() = default;

    // 基础日志接口
    virtual void log(LogLevel level, const std::string& msg) = 0;

    // 格式化日志接口
    virtual void logf(LogLevel level, const char* fmt, ...) = 0;

    // 便捷方法
    virtual void trace(const std::string& msg) = 0;
    virtual void debug(const std::string& msg) = 0;
    virtual void info(const std::string& msg) = 0;
    virtual void warn(const std::string& msg) = 0;
    virtual void error(const std::string& msg) = 0;
    virtual void critical(const std::string& msg) = 0;

    // 带格式的便捷方法
    virtual void tracef(const char* fmt, ...) = 0;
    virtual void debugf(const char* fmt, ...) = 0;
    virtual void infof(const char* fmt, ...) = 0;
    virtual void warnf(const char* fmt, ...) = 0;
    virtual void errorf(const char* fmt, ...) = 0;
    virtual void criticalf(const char* fmt, ...) = 0;

    // 配置方法
    virtual void set_level(LogLevel level) = 0;
    virtual void flush() = 0;
};

