#include "Settings.h"
#include "ShortestPath.h"
#include "DataPipeline.h"

Logger logger = {
        .level = DEBUG,
        .debug = debug,
        .enable_color = true,
        .enable_level_name = false,
        .enable_date = false,
        .enable_time = true,
        .info = info,
        .warn = warn,
        .error = error,
};


void log_function(enum LogLevel level, const char *format, va_list args) {
    if (level < logger.level)
        return;

    if (logger.enable_color) {
        switch (level) {
            case DEBUG:
                printf("\033[90m");
                break;
            case INFO:
                printf("\033[36m");
                break;
            case WARN:
                printf("\033[0;33m");
                break;
            case ERROR:
                printf("\033[0;31m");
                break;
            default:
                break;
        }
    }

    if (strcmp(format, "---") == 0) {
        printf("----------------------------------------------------------\n");
        return;
    }

    if (logger.enable_date || logger.enable_time) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        printf("[");
        if (logger.enable_date) {
            printf("%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
        }
        if (logger.enable_date && logger.enable_time) {
            printf(" ");
        }
        if (logger.enable_time) {
            printf("%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
        printf("]");
    }

    if (logger.enable_level_name) {
        switch (level) {
            case DEBUG:
                printf("[DEBUG] ");
                break;
            case INFO:
                printf("[INFO] ");
                break;
            case WARN:
                printf("[WARN] ");
                break;
            case ERROR:
                printf("[ERROR] ");
                break;
            default:
                break;
        }
    } else if (logger.enable_date || logger.enable_time) {
        printf(" ");
    }

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd': {
                    int val = va_arg(args, int);
                    printf("%d", val);
                    break;
                }
                    // Handle double using .nf
                case '.': {
                    format++;
                    int precision = *format - '0';
                    format++;
                    if (*format == 'f') {
                        double val = va_arg(args, double);
                        printf("%.*f", precision, val);
                    }
                    break;
                }
                    // Handle simple double
                case 'f': {
                    double val = va_arg(args, double);
                    printf("%f", val);
                    break;
                }
                case 's': {
                    char *val = va_arg(args, char*);
                    printf("%s", val);
                    break;
                }
                case 'c': {
                    char val = (char)va_arg(args, int);
                    printf("%c", val);
                    break;
                }
                case 'p': {
                    Path *val = va_arg(args, Path*);
                    Path_print(val);
                    break;
                }
                case 'a': {
                    Address *val = va_arg(args, Address*);
                    Address_print(val);
                    break;
                }
                    //case 'n': {
                    //    GraphNode* val = va_arg(args, Path*);
                    //    Path_print(val);
                    //    break;
                    //}
                default:
                    printf("\n\n\033[0;31mError: unknown format specifier %%%c\033[0m\n\n", *format);
                    _Exit(1);
            }
        } else {
            putchar(*format);
        }
        format++;
    }

    if (logger.enable_color) {
        printf("\033[0m");
    }
}

void debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_function(DEBUG, format, args);
    va_end(args);
}

void info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_function(INFO, format, args);
    va_end(args);
}

void warn(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_function(WARN, format, args);
    va_end(args);
}

void error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_function(ERROR, format, args);
    va_end(args);
}
