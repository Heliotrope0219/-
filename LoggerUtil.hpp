//
// Created by Kai Lee on 2025/8/1.
//

#pragma once

#include <string>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/stat.h>
#include <direct.h>
#include "../ILogger.h"

/**
 * @brief LoggerUtil 工具函数
 */
class LoggerUtil {
public:
    static std::string get_current_date(const std::string &format) {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);

        std::ostringstream oss;
        oss << std::put_time(&tm, format.c_str());
        return oss.str();
    }

    static std::string level_to_string(LogLevel level) {
        switch (level) {
            case LogLevel::trace:
                return "TRACE";
            case LogLevel::debug:
                return "DEBUG";
            case LogLevel::info:
                return "INFO";
            case LogLevel::warn:
                return "WARN";
            case LogLevel::error:
                return "ERROR";
            case LogLevel::critical:
                return "CRITICAL";
            default:
                return "UNKNOWN";
        }
    }

    static bool ensure_dir_exists(const std::string &path) {
        struct stat info{};
        if (stat(path.c_str(), &info) != 0) {
            // 目录不存在，尝试创建
#if defined(_WIN32)
            return mkdir(path.c_str()) == 0;
#else
            return mkdir(path.c_str(), 0755) == 0;
#endif
        } else if (info.st_mode & S_IFDIR) {
            // 目录已存在
            return true;
        }
        return false;
    }
};
