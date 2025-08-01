//
// Created by Kai Lee on 2025/8/1.
//

#include "Logger.h"
#include <cstdarg>
#include <cstdio>
#include <vector>

std::shared_ptr<Logger> Logger::create(
        const std::string& name,
        OutputTarget target,
        const std::string& base_path,
        LogLevel level,
        size_t max_file_size,
        size_t max_files
) {
    return std::shared_ptr<Logger>(new Logger(
            name, target, base_path, level, max_file_size, max_files
    ));
}

Logger::Logger(
        const std::string& name,
        OutputTarget target,
        const std::string& base_path,
        LogLevel level,
        size_t max_file_size,
        size_t max_files
) : name_(name), target_(target) {

    std::vector<spdlog::sink_ptr> sinks;
    const std::string date_dir = LoggerUtil::get_current_date("%Y%m%d");
    const std::string log_path = base_path + "/" + date_dir + "/" + name + ".log";

    // 创建控制台sink（带颜色）
    if (target == OutputTarget::CONSOLE_ONLY || target == OutputTarget::BOTH) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(to_spdlog_level(level));
#ifdef LOG_DEBUG_OUTPUT
        // 调试模式：简洁格式
        console_sink->set_pattern("[%^%l%$] [%n] %v");
#else
        // 生产模式：详细格式
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");
#endif
        sinks.push_back(console_sink);
    }

    // 创建文件sink（自动轮转）
    if (target == OutputTarget::FILE_ONLY || target == OutputTarget::BOTH) {
        // 确保目录存在
        LoggerUtil::ensure_dir_exists(base_path);
        LoggerUtil::ensure_dir_exists(base_path + "/" + date_dir);

        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                log_path, max_file_size, max_files);
        file_sink->set_level(to_spdlog_level(level));
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] [thread %t] %v");
        sinks.push_back(file_sink);
    }

    // 创建组合logger
    logger_ = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    logger_->set_level(to_spdlog_level(level));
    logger_->flush_on(spdlog::level::err);  // 错误级别立即刷新
}

void Logger::log(LogLevel level, const std::string& msg) {
    if (!logger_) return;

    // 调试模式下同时输出到控制台
#ifdef LOG_DEBUG_OUTPUT
    if (target_ != CONSOLE_ONLY) {
        std::cout << "[" << LoggerUtil::level_to_string(level) << "] "
                  << "[" << name_ << "] " << msg << std::endl;
    }
#endif

    logger_->log(to_spdlog_level(level), msg);
}

void Logger::logf(LogLevel level, const char* fmt, ...) {
    if (!logger_) return;

    va_list args;
            va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
            va_end(args);

    log(level, buffer);
}

// 实现变参模板的便捷方法
#define IMPLEMENT_FORMATTED_LOG(level) \
void Logger::level##f(const char* fmt, ...) { \
    va_list args; \
    va_start(args, fmt); \
    char buffer[1024]; \
    vsnprintf(buffer, sizeof(buffer), fmt, args); \
    va_end(args); \
    log(LogLevel::level, buffer); \
}

IMPLEMENT_FORMATTED_LOG(trace)
IMPLEMENT_FORMATTED_LOG(debug)
IMPLEMENT_FORMATTED_LOG(info)
IMPLEMENT_FORMATTED_LOG(warn)
IMPLEMENT_FORMATTED_LOG(error)
IMPLEMENT_FORMATTED_LOG(critical)

#undef IMPLEMENT_FORMATTED_LOG

void Logger::set_level(LogLevel level) {
    if (logger_) {
        logger_->set_level(to_spdlog_level(level));
    }
}

void Logger::flush() {
    if (logger_) {
        logger_->flush();
    }
}

spdlog::level::level_enum Logger::to_spdlog_level(LogLevel level) {
    switch (level) {
        case LogLevel::trace: return spdlog::level::trace;
        case LogLevel::debug: return spdlog::level::debug;
        case LogLevel::info: return spdlog::level::info;
        case LogLevel::warn: return spdlog::level::warn;
        case LogLevel::error: return spdlog::level::err;
        case LogLevel::critical: return spdlog::level::critical;
        default: return spdlog::level::info;
    }
}

LogLevel Logger::from_spdlog_level(spdlog::level::level_enum level) {
    switch (level) {
        case spdlog::level::trace: return LogLevel::trace;
        case spdlog::level::debug: return LogLevel::debug;
        case spdlog::level::info: return LogLevel::info;
        case spdlog::level::warn: return LogLevel::warn;
        case spdlog::level::err: return LogLevel::error;
        case spdlog::level::critical: return LogLevel::critical;
        default: return LogLevel::info;
    }
}