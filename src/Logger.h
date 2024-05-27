#pragma once
#include "Settings.h"

enum LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    // This state is used to hide all the log
    HIDDEN,
};


typedef struct {
    enum LogLevel level;
    bool enable_color;
    bool enable_level_name;
    bool enable_date;
    bool enable_time;
    void (*log_function)(enum LogLevel level, const char* format, va_list args);
    void (*debug)(const char* format, ...);
    void (*info)(const char* format, ...);
    void (*warn)(const char* format, ...);
    void (*error)(const char* format, ...);
} Logger;

extern Logger logger;

/**
 * Log functions
 * @param format the format string
 * @param ...   the arguments
 *
 */
void debug(const char* format, ...);
void info(const char* format, ...);
void warn(const char* format, ...);
void error(const char* format, ...);

/**
 * Log function
 * @param level the log level
 * @param format the format string
 * @param args the arguments
 * @return void
 *
 * This function is used to log the messages to the console.
 */
void log_function(enum LogLevel level, const char* format, va_list args);

