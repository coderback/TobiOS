/**
 * @file logger.h
 * @brief Multi-level logging system for TobiOS
 * 
 * This file defines the Logger class which provides colored, timestamped
 * logging functionality with configurable log levels and optional file output.
 */

#pragma once

#include <string>
#include <fstream>
#include <chrono>
#include <iostream>

namespace TobiOS {

/**
 * @enum LogLevel
 * @brief Enumeration of available log levels in order of severity
 * 
 * Log levels control which messages are displayed based on their importance.
 * Lower numbered levels include all higher numbered levels.
 */
enum class LogLevel {
    DEBUG = 0,      ///< Detailed debugging information
    INFO = 1,       ///< General informational messages
    WARNING = 2,    ///< Warning messages for potential issues
    ERROR = 3,      ///< Error messages for recoverable failures
    CRITICAL = 4    ///< Critical errors that may cause system failure
};

/**
 * @class Logger
 * @brief Centralized logging system with colored output and file support
 * 
 * The Logger class provides a unified logging interface for the entire OS.
 * Features include:
 * - Colored console output based on log level
 * - Timestamped messages
 * - Configurable minimum log level filtering
 * - Optional file output for persistent logging
 * - Thread-safe logging operations
 */
class Logger {
public:
    /**
     * @brief Constructor - initializes logger with default settings
     * Sets log level to INFO and disables file logging by default
     */
    Logger();
    
    /**
     * @brief Destructor - ensures file logging is properly closed
     */
    ~Logger();

    /**
     * @brief Set the minimum log level for message filtering
     * @param level Minimum LogLevel - messages below this level are ignored
     */
    void setLogLevel(LogLevel level);
    
    /**
     * @brief Enable logging to a file in addition to console output
     * @param filename Path to the log file to write to
     */
    void enableFileLogging(const std::string& filename);
    
    /**
     * @brief Disable file logging (console logging continues)
     */
    void disableFileLogging();

    /**
     * @brief Log a message with specified level
     * @param level LogLevel of the message
     * @param message Text content to log
     */
    void log(LogLevel level, const std::string& message);
    
    /**
     * @brief Log a debug message (lowest priority)
     * @param message Debug message content
     */
    void debug(const std::string& message);
    
    /**
     * @brief Log an informational message
     * @param message Info message content
     */
    void info(const std::string& message);
    
    /**
     * @brief Log a warning message
     * @param message Warning message content
     */
    void warning(const std::string& message);
    
    /**
     * @brief Log an error message
     * @param message Error message content
     */
    void error(const std::string& message);
    
    /**
     * @brief Log a critical error message (highest priority)
     * @param message Critical message content
     */
    void critical(const std::string& message);

private:
    /**
     * @brief Get current timestamp as formatted string
     * @return Timestamp string in format "YYYY-MM-DD HH:MM:SS"
     */
    std::string getCurrentTimestamp();
    
    /**
     * @brief Convert LogLevel enum to string representation
     * @param level LogLevel to convert
     * @return String representation of the log level
     */
    std::string levelToString(LogLevel level);
    
    /**
     * @brief Get ANSI color code for the specified log level
     * @param level LogLevel to get color for
     * @return ANSI color escape sequence
     */
    std::string getColorCode(LogLevel level);

    LogLevel currentLevel_;        ///< Current minimum log level filter
    bool fileLoggingEnabled_;     ///< Flag indicating if file logging is active
    std::ofstream logFile_;       ///< File stream for log output
};

}