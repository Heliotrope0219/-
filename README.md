# 日志系统使用说明

## 概述

本日志系统是一个基于​**​spdlog库​**​构建的高性能C++日志框架，通过封装spdlog的核心功能，提供了更加便捷易用的接口和增强特性。系统提供多级别日志记录、多输出目标支持以及智能文件管理功能，适用于从开发调试到生产部署的全场景需求。
在使用时只需包含`LoggerManager.h`头文件，通过`LoggerManager`获取`ILogger`接口的日志器实例，然后使用`ILogger`提供的各种日志方法即可。内部实现类（如Logger、LoggerUtil等）已封装在项目中，用户无需直接引用。

> ⚠️ ​**​核心依赖​**​  
> 本系统​**​强依赖spdlog库​**​（v1.x或更高版本），请确保在项目中正确包含spdlog头文件和链接库。

## 核心特性

- ​**​spdlog基础​**​：构建在spdlog的高性能异步日志框架之上
- ​**​六级日志体系​**​：支持trace到critical的完整日志级别
- ​**​双模式输出​**​：控制台(带颜色高亮) + 文件(带自动轮转)
- ​**​智能文件管理​**​：按日期分目录 + 大小/数量双重限制
- ​**​线程安全​**​：所有操作均保证线程安全
- ​**​零配置开箱即用​**​：默认提供系统日志器
- **​最小依赖原则​**​：用户只需包含 `LoggerManager.h` 头文件
- **​接口导向​**​：所有操作通过 `ILogger` 接口进行，该接口展示了完整的日志API
- **​封装实现​**​：`Logger`、`LoggerUtil` 等内部类已封装在项目中，用户无需关注其实现细节
- **​开箱即用​**​：通过 `LoggerManager::get_logger()` 或 `get_system_logger()` 可直接获取即用型日志器
## 依赖要求

1. ​**​必需组件​**​：

   - [spdlog](https://github.com/gabime/spdlog) v1.8.5+
   - C++11或更高标准
2. ​**​推荐组件​**​：

   - [fmt](https://github.com/fmtlib/fmt) (用于高级格式化)
   - CMake (构建系统)

## 日志级别说明（基于spdlog级别映射）

|级别|spdlog对应级别|数值|使用场景|生产环境建议|
|---|---|---|---|---|
|`trace`|`spdlog::level::trace`|0|最详细的执行路径跟踪|关闭|
|`debug`|`spdlog::level::debug`|1|调试信息，变量状态检查|按需开启|
|`info`|`spdlog::level::info`|2|关键流程节点信息，系统运行状态|开启|
|`warn`|`spdlog::level::warn`|3|非预期但可恢复的异常情况|开启|
|`error`|`spdlog::level::err`|4|部分功能不可用，但系统仍可运行|开启|
|`critical`|`spdlog::level::critical`|5|系统级错误，需要立即人工干预|开启|

> 注意：`error`及以上级别会立即刷新缓冲区确保日志持久化

## 获取日志器

### 1. 获取系统默认日志器

```
#include "Logs/LoggerManager.h"

auto logger = LoggerManager::get_system_logger();
logger->info("System logger initialized");
```

### 2. 获取自定义日志器

```
auto dbLogger = LoggerManager::get_logger(
    "database",                  // 日志器名称
    Logger::OutputTarget::FILE_ONLY, // 输出目标
    "/var/logs",                 // 基础路径
    LogLevel::debug,             // 日志级别
    10 * 1024 * 1024,            // 最大文件大小(10MB)
    7                            // 最大文件数量
);
```

## 日志记录方法

### 1.基本日志接口

```
// 直接记录
logger->info("User login: " + username);

// 格式化记录
logger->errorf("Connection failed to %s:%d", ip, port);
```

### 2.级别快捷方法

```
logger->trace("Entering critical section");
logger->debugf("Buffer size: %zu", buffer.size());
logger->info("Server started on port 8080");
logger->warn("Cache miss for key: " + key);
logger->error("Database connection lost");
logger->criticalf("Disk space critical: %dMB left", freeSpace);
```

## spdlog特性集成

### 1. 输出目标配置

```
enum class OutputTarget {
    CONSOLE_ONLY,  // 仅控制台（使用spdlog::sinks::stdout_color_sink_mt）
    FILE_ONLY,     // 仅文件（使用spdlog::sinks::rotating_file_sink_mt）
    BOTH           // 同时输出
};
```

### 2. 格式模式

```
// 控制台输出（开发模式简洁格式）
[INFO] [system] Starting service

// 控制台输出（生产模式详细格式）
[2025-08-01 15:33:21.789] [INFO] [system] Starting service

// 文件输出格式（固定详细格式）
[2025-08-01 15:30:45.123] [INFO] [system] [thread 12524] Application started
```

## 文件管理策略

|参数|默认值|说明|
|---|---|---|
|`base_path`|"logs"|日志存储基础目录|
|`max_file_size`|5MB|单个日志文件最大尺寸|
|`max_files`|3|保留的历史日志文件数量|

文件路径格式：  
`{base_path}/{YYYYMMDD}/{logger_name}.log`  
示例：  
`logs/20250801/system.log`

## 高级操作

### 1.动态调整日志级别

```
// 生产环境临时开启调试
dbLogger->set_level(LogLevel::debug);

// 流量高峰期间降低日志负载
webLogger->set_level(LogLevel::warn);
```

### 2.手动刷新缓冲区

```
try {
    // ... 关键操作 ...
} catch (...) {
    logger->critical("Unhandled exception!");
    logger->flush(); // 确保异常信息立即持久化
}
```

### 3.访问底层spdlog对象

```
// 获取底层spdlog记录器
auto spd_logger = std::dynamic_pointer_cast<Logger>(logger)->get_spdlog_logger();

// 直接使用spdlog原生功能
spd_logger->flush();
spd_logger->set_level(spdlog::level::debug);
```

## 性能优化建议

1. ​**​异步日志​**​：在spdlog配置中启用异步日志

    ```
    // 在Logger.cpp中可修改为
    auto logger = std::make_shared<spdlog::async_logger>(...);
    ```

2. ​**​批量写入​**​：通过`flush_on`控制刷新频率

    ```
    // 仅错误级别立即刷新
    logger_->flush_on(spdlog::level::err);
    ```

3. ​**​格式优化​**​：简化生产环境格式提高性能

    ```
    file_sink->set_pattern("%L %v"); // 最简格式
    ```


## 示例代码

```
#include "LoggerManager.h"

void processTransaction(int amount) {
    auto logger = LoggerManager::get_logger("transaction");
    
    logger->tracef("Starting transaction: %d", amount);
    
    if (amount > 10000) {
        logger->warn("Large transaction amount: " + std::to_string(amount));
    }
    
    try {
        // ... 业务逻辑 ...
        logger->info("Transaction completed");
    } catch (const std::exception& e) {
        logger->errorf("Transaction failed: %s", e.what());
        logger->flush();
        throw;
    }
}

int main() {
    auto sysLogger = LoggerManager::get_system_logger();
    sysLogger->info("Application started");
    
    // 配置网络模块日志
    auto netLogger = LoggerManager::get_logger(
        "network", 
        Logger::OutputTarget::BOTH,
        "logs/network",
        LogLevel::debug
    );
    
    netLogger->debug("Initializing network module");
    processTransaction(15000);
    
    sysLogger->info("Application shutdown");
    return 0;
}
```

## 输出示例

### 1.控制台输出（开发模式）

```
[INFO] [system] Application started
[DEBUG] [network] Initializing network module
[WARN] [transaction] Large transaction amount: 15000
[INFO] [transaction] Transaction completed
```

### 2.文件输出内容

```
[2025-08-01 16:30:45.123] [INFO] [system] [thread 12524] Application started
[2025-08-01 16:30:45.567] [DEBUG] [network] [thread 12524] Initializing network module
[2025-08-01 16:30:46.235] [WARN] [transaction] [thread 12531] Large transaction amount: 15000
[2025-08-01 16:30:47.189] [INFO] [transaction] [thread 12531] Transaction completed
```

## 兼容性与注意事项

1. ​**​spdlog版本​**​：兼容spdlog 1.x所有版本
2. ​**​平台支持​**​：Windows/Linux/macOS全平台
3. ​**​异常处理​**​：所有spdlog异常在内部捕获并处理
4. ​**​线程安全​**​：
   - 所有日志操作均线程安全
   - 可跨线程共享同一日志器实例
5. ​**​性能影响​**​：
   - `trace`/`debug`级别日志在生产环境应关闭
   - 避免在热点路径中拼接复杂字符串
6. ​**​文件管理​**​：
   - 每日自动创建新目录
   - 文件轮转发生在日志写入时
   - 超过`max_files`的最旧文件会被删除

## 故障排除

​**​Q：链接时报spdlog符号未定义错误​**​  
A：确保正确链接spdlog库，检查`find_package(spdlog)`是否成功

​**​Q：文件创建失败​**​  
A：检查目录权限，使用`LoggerUtil::ensure_dir_exists`确保目录存在

​**​Q：控制台无彩色输出​**​  
A：确认终端支持ANSI颜色，或检查`spdlog::sinks::stdout_color_sink_mt`是否正确创建

​**​Q：日志轮转不工作​**​  
A：检查`max_file_size`设置是否合理，确保有写入权限

---

通过封装spdlog的核心功能，本日志系统在保持spdlog高性能优势的同时，提供了更加易用和业务友好的接口，适合各类C++项目集成使用。系统通过合理的默认配置和灵活的定制能力，平衡了开发便利性与生产环境要求，为应用提供可靠的日志记录解决方案。
