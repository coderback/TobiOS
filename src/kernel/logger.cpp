/**
 * @file logger.cpp
 * @brief Implementation of the TobiOS logging system
 * 
 * This file contains the implementation of the Logger class which provides
 * colored, timestamped logging with configurable levels and optional file output.
 */

#include "logger.h"
#include <iomanip>
#include <sstream>

namespace TobiOS {

Logger::Logger() : currentLevel_(LogLevel::INFO), fileLoggingEnabled_(false) {}

Logger::~Logger() {
    // Ensure log file is properly closed on destruction
    if (fileLoggingEnabled_ && logFile_.is_open()) {
        logFile_.close();
    }
}

void Logger::setLogLevel(LogLevel level) {
    // Update the minimum log level filter
    currentLevel_ = level;
}

void Logger::enableFileLogging(const std::string& filename) {
    // Open log file in append mode to preserve existing logs
    logFile_.open(filename, std::ios::app);
    if (logFile_.is_open()) {
        fileLoggingEnabled_ = true;
        info("File logging enabled: " + filename);
    } else {
        error("Failed to enable file logging: " + filename);
    }
}

void Logger::disableFileLogging() {
    // Disable file logging and close the file handle
    if (fileLoggingEnabled_ && logFile_.is_open()) {
        info("File logging disabled");
        logFile_.close();
        fileLoggingEnabled_ = false;
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    // Filter messages based on current log level
    if (static_cast<int>(level) < static_cast<int>(currentLevel_)) {
        return;
    }

    // Build formatted log message with timestamp and level
    std::string timestamp = getCurrentTimestamp();
    std::string levelStr = levelToString(level);
    std::string colorCode = getColorCode(level);
    
    std::string logMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    // Output to console with color coding
    std::cout << colorCode << logMessage << "\033[0m" << std::endl;
    
    // Also write to file if file logging is enabled (without color codes)
    if (fileLoggingEnabled_ && logFile_.is_open()) {
        logFile_ << logMessage << std::endl;
        logFile_.flush(); // Force immediate write to disk
    }
}

void Logger::debug(const std::string& message) {
    // Wrapper for DEBUG level logging
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    // Wrapper for INFO level logging
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    // Wrapper for WARNING level logging
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    // Wrapper for ERROR level logging
    log(LogLevel::ERROR, message);
}

void Logger::critical(const std::string& message) {
    // Wrapper for CRITICAL level logging
    log(LogLevel::CRITICAL, message);
}

std::string Logger::getCurrentTimestamp() {
    // Get current time with millisecond precision
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    // Format as HH:MM:SS.mmm
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
    // Convert LogLevel enum to fixed-width string for consistent formatting
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO ";
        case LogLevel::WARNING: return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRIT ";
        default: return "UNKN ";
    }
}

std::string Logger::getColorCode(LogLevel level) {
    // Return ANSI color escape sequences for different log levels
    switch (level) {
        case LogLevel::DEBUG: return "\033[36m";    // Cyan - for debug info
        case LogLevel::INFO: return "\033[37m";     // White - for normal info
        case LogLevel::WARNING: return "\033[33m";  // Yellow - for warnings
        case LogLevel::ERROR: return "\033[31m";    // Red - for errors
        case LogLevel::CRITICAL: return "\033[35m"; // Magenta - for critical errors
        default: return "\033[37m";                 // White - fallback
    }
}

}