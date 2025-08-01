
#include "LoggerManager.hpp"
#include <thread>
#include <vector>
#include <chrono>

// 模拟业务模块函数
void processOrder(int orderId, double amount) {
    auto logger = LoggerManager::get_logger("order");

    logger->tracef("[Order-%d] Processing started", orderId);

    // 模拟处理过程
    logger->debugf("[Order-%d] Validating amount: %.2f", orderId, amount);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    if (amount > 5000.0) {
        logger->warnf("[Order-%d] High value order: %.2f", orderId, amount);
    }

    try {
        if (orderId % 10 == 0) {
            throw std::runtime_error("Payment gateway timeout");
        }

        logger->infof("[Order-%d] Processed successfully", orderId);
    } catch (const std::exception& e) {
        logger->errorf("[Order-%d] Processing failed: %s", orderId, e.what());
        logger->flush(); // 立即刷新确保错误日志持久化
    }

    logger->tracef("[Order-%d] Processing completed", orderId);
}

// 模拟网络模块函数
void fetchData(const std::string& url) {
    auto logger = LoggerManager::get_logger("network");

    logger->debugf("Connecting to %s", url.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(15));

    if (url.find("secure") != std::string::npos) {
        logger->infof("Secure connection established to %s", url.c_str());
    } else {
        logger->warn("Insecure connection, consider using HTTPS");
    }

    logger->debugf("Data fetched from %s", url.c_str());
}

// 模拟系统关键操作
void criticalSystemOperation() {
    auto sysLogger = LoggerManager::get_system_logger();

    sysLogger->info("Starting critical system operation");

    try {
        // 模拟关键操作
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        // 模拟偶发的严重错误
        if (rand() % 100 < 5) { // 5% 概率模拟失败
            throw std::runtime_error("Hardware failure detected");
        }

        sysLogger->info("Critical operation completed successfully");
    } catch (const std::exception& e) {
        sysLogger->criticalf("Critical operation failed: %s", e.what());
        sysLogger->flush();
    }
}

int main() {
    // 获取系统日志器
    auto sysLogger = LoggerManager::get_system_logger();
    sysLogger->info("Application starting...");

    // 创建自定义日志器（文件大小限制为1MB，仅保留2个文件）
    auto fileLogger = LoggerManager::get_logger(
            "file_only",
            Logger::OutputTarget::FILE_ONLY,
            "logs/demo",
            LogLevel::trace,
            1 * 1024 * 1024, // 1MB
            2                // 保留2个文件
    );

    fileLogger->info("This log will go to file only");

    // 记录所有级别的日志
    sysLogger->trace("This is a TRACE message");
    sysLogger->debug("This is a DEBUG message");
    sysLogger->info("This is an INFO message");
    sysLogger->warn("This is a WARN message");
    sysLogger->error("This is an ERROR message");
    sysLogger->critical("This is a CRITICAL message");

    // 使用格式化日志
    sysLogger->infof("Formatted log: %s %d %.2f", "example", 42, 3.14159);

    // 创建多个线程模拟并发日志记录
    sysLogger->info("Starting worker threads...");
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i] {
            auto threadLogger = LoggerManager::get_logger("worker");
            threadLogger->infof("Thread %d started", i);

            // 处理订单
            for (int j = 0; j < 20; ++j) {
                processOrder(i * 100 + j, 100.0 + j * 250.0);
            }

            threadLogger->infof("Thread %d completed", i);
        });
    }

    // 在主线程执行网络操作
    for (int i = 0; i < 10; ++i) {
        fetchData(i % 2 == 0 ? "http://api.example.com" : "https://secure.api.example.com");
    }

    // 执行关键系统操作
    for (int i = 0; i < 3; ++i) {
        criticalSystemOperation();
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    // 动态调整日志级别
    sysLogger->warn("Temporarily lowering log level to WARN due to high load");
    sysLogger->set_level(LogLevel::warn);

    // 这些日志不会显示
    sysLogger->debug("This debug message should not appear");
    sysLogger->info("This info message should not appear");

    // 恢复日志级别
    sysLogger->set_level(LogLevel::info);
    sysLogger->info("Log level restored to INFO");

    // 文件轮转测试 - 生成大日志文件
    sysLogger->info("Starting file rotation test...");
    for (int i = 0; i < 5000; ++i) {
        fileLogger->infof("Log rotation test message %d - %s", i,
                          "This is a long message to fill up the log file quickly. "
                          "It should demonstrate the log rotation feature.");
    }
    sysLogger->info("File rotation test completed");

    sysLogger->info("Application shutting down");
    return 0;
}