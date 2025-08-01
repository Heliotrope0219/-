//
// Created by Kai Lee on 2025/8/1.
//

#pragma once

#include "../ILogger.h"
#include "LoggerUtil.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <memory>
#include <vector>


class Logger : public ILogger {
public:
    enum class OutputTarget {
        CONSOLE_ONLY,
        FILE_ONLY,
        BOTH
    };

    // 创建日志器实例
    static std::shared_ptr<Logger> create(
            const std::string &name,
            OutputTarget target = OutputTarget::BOTH,
            const std::string &base_path = "logs",
            LogLevel level = LogLevel::info,
            size_t max_file_size = 5 * 1024 * 1024,
            size_t max_files = 3
    );

    // 接口实现
    void log(LogLevel level, const std::string &msg) override;

    void logf(LogLevel level, const char *fmt, ...) override;

    void trace(const std::string &msg) override { log(LogLevel::trace, msg); }

    void debug(const std::string &msg) override { log(LogLevel::debug, msg); }

    void info(const std::string &msg) override { log(LogLevel::info, msg); }

    void warn(const std::string &msg) override { log(LogLevel::warn, msg); }

    void error(const std::string &msg) override {
        log(LogLevel::error, msg);
    }

    void critical(const std::string &msg) override { log(LogLevel::critical, msg); }

    void tracef(const char *fmt, ...) override;

    void debugf(const char *fmt, ...) override;

    void infof(const char *fmt, ...) override;

    void warnf(const char *fmt, ...) override;

    void errorf(const char *fmt, ...) override;

    void criticalf(const char *fmt, ...) override;

    void set_level(LogLevel level) override;

    void flush() override;

private:
    Logger(
            const std::string &name,
            OutputTarget target,
            const std::string &base_path,
            LogLevel level,
            size_t max_file_size,
            size_t max_files
    );

    spdlog::level::level_enum to_spdlog_level(LogLevel level);

    LogLevel from_spdlog_level(spdlog::level::level_enum level);

    std::shared_ptr<spdlog::logger> logger_;
    std::string name_;
    OutputTarget target_;
};

