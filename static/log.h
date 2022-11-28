#ifndef STATIC_LOG_H
#define STATIC_LOG_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>

#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_FATAL 5
#define LOG_LEVEL_NONE 6

#define LOG_FORMAT_(function, message, size, ...) {\
    int position = function(message+size, LOG_BUFFER_SIZE-size, __VA_ARGS__);\
    if(position >= 0) size += position; \
}

#ifndef LOG_FORMAT_PREFIX
    #define LOG_FORMAT_PREFIX(message, size, level, file, function, line, ...) {\
        static const char * prefix[] = { "TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL" };\
        LOG_FORMAT_(snprintf, message, size, "%s", prefix[level]);\
    }
#endif

#ifndef LOG_FORMAT_TIME
    #define LOG_FORMAT_TIME(message, size, level, file, function, line, ...) {\
        struct timespec time_spec;\
        clock_gettime(CLOCK_REALTIME, &time_spec);\
        struct tm * time_info = localtime(&time_spec.tv_sec);\
        LOG_FORMAT_(strftime, message, size, "%d.%m.%Y %H:%M:%S:", time_info);\
        LOG_FORMAT_(snprintf, message, size, "%.3ld", time_spec.tv_nsec / 1000000);\
    }
#endif

#ifndef LOG_FORMAT_LOCATION
    #define LOG_FORMAT_LOCATION(message, size, level, file, function, line, ...) {\
        const char * last_slash = strrchr(file, '/');\
        const char * file_name = last_slash ? last_slash + 1 : file;\
        LOG_FORMAT_(snprintf, message, size, "%s:%s:%d", file_name, function, line);\
    }
#endif 

#ifndef LOG_FORMAT_MESSAGE
    #define LOG_FORMAT_MESSAGE(message, size, level, file, function, line, ...) {\
        LOG_FORMAT_PREFIX(message, size, level, file, function, line, __VA_ARGS__);\
        LOG_FORMAT_(snprintf, message, size, " ");\
        LOG_FORMAT_TIME(message, size, level, file, function, line, __VA_ARGS__);\
        LOG_FORMAT_(snprintf, message, size, " ");\
        LOG_FORMAT_LOCATION(message, size, level, file, function, line, __VA_ARGS__);\
        LOG_FORMAT_(snprintf, message, size, " ");\
        LOG_FORMAT_(snprintf, message, size, __VA_ARGS__);\
    }
#endif

#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_TRACE
#endif

#ifndef LOG_BUFFER_SIZE
    #define LOG_BUFFER_SIZE 128
#endif

#define LOG_START(level, file, function, line, ...) {\
    int _log_size=0;\
    static char _log_message[LOG_BUFFER_SIZE];\
    LOG_FORMAT_MESSAGE(_log_message, _log_size, level, file, function, line, __VA_ARGS__);\
    fprintf(stderr, "%s\n", _log_message);\
}
        
#if LOG_LEVEL <= LOG_LEVEL_FATAL
    #define LOG_FATAL(...) LOG_START(LOG_LEVEL_FATAL, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define LOG_FATAL(...) 
#endif 

#if LOG_LEVEL <= LOG_LEVEL_ERROR
    #define LOG_ERROR(...) LOG_START(LOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define LOG_ERROR(...) 
#endif 

#if LOG_LEVEL <= LOG_LEVEL_WARN
    #define LOG_WARN(...) LOG_START(LOG_LEVEL_WARN, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define LOG_WARN(...) 
#endif 

#if LOG_LEVEL <= LOG_LEVEL_INFO
    #define LOG_INFO(...) LOG_START(LOG_LEVEL_INFO, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define LOG_INFO(...) 
#endif 

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
    #define LOG_DEBUG(...) LOG_START(LOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define LOG_DEBUG(...) 
#endif 

#if LOG_LEVEL <= LOG_LEVEL_TRACE
    #define LOG_TRACE(...) LOG_START(LOG_LEVEL_TRACE, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define LOG_TRACE(...) 
#endif 

#ifdef __cplusplus
}
#endif
#endif /* STATIC_LOG_H */
