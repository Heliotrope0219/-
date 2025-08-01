//
// Created by Kai Lee on 2025/8/1.
//

#pragma once

#include "Logs/Logger.h"
#include <map>
#include <mutex>
#include <memory>
#include <string>

/**
 * @brief 日志管理器
 */
class LoggerManager {
public:
    static std::shared_ptr<ILogger> get_logger(
            const std::string& name,
            Logger::OutputTarget target = Logger::OutputTarget::BOTH,
            const std::string& base_path = "logs",
            LogLevel level = LogLevel::info,
            size_t max_file_size = 5 * 1024 * 1024,
            size_t max_files = 3
    ) {
        static std::mutex mutex;
        static std::map<std::string, std::shared_ptr<ILogger>> loggers;

        std::lock_guard<std::mutex> lock(mutex);

        auto it = loggers.find(name);
        if (it != loggers.end()) {
            return it->second;
        }

        auto new_logger = Logger::create(
                name, target, base_path, level, max_file_size, max_files
        );

        loggers[name] = new_logger;
        return new_logger;
    }

    static std::shared_ptr<ILogger> get_system_logger() {
        static auto system_logger = get_logger("system");
        return system_logger;
    }
};
