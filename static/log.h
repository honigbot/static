#ifndef STATIC_LOG_H
#define STATIC_LOG_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>

#define STATIC_LOG_LEVEL_TRACE 0
#define STATIC_LOG_LEVEL_DEBUG 1
#define STATIC_LOG_LEVEL_INFO 2
#define STATIC_LOG_LEVEL_WARN 3
#define STATIC_LOG_LEVEL_ERROR 4
#define STATIC_LOG_LEVEL_FATAL 5
#define STATIC_LOG_LEVEL_NONE 6

#define STATIC_LOG_FORMAT_(function, message, size, ...) {\
    int position = function(message+size, STATIC_LOG_BUFFER_SIZE-size, __VA_ARGS__);\
    if(position >= 0) size += position; \
}

#ifndef STATIC_LOG_FORMAT_PREFIX
    #define STATIC_LOG_FORMAT_PREFIX(message, size, level, file, function, line, format, ...) {\
        static const char * prefix[] = { "TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL" };\
        STATIC_LOG_FORMAT_(snprintf, message, size, "%s", prefix[level]);\
    }
#endif

#ifndef STATIC_LOG_FORMAT_TIME
    #define STATIC_LOG_FORMAT_TIME(message, size, level, file, function, line, format, ...) {\
        struct timespec time_spec;\
        clock_gettime(CLOCK_REALTIME, &time_spec);\
        struct tm * time_info = localtime(&time_spec.tv_sec);\
        STATIC_LOG_FORMAT_(strftime, message, size, "%d.%m.%Y %H:%M:%S:", time_info);\
        STATIC_LOG_FORMAT_(snprintf, message, size, "%.3ld", time_spec.tv_nsec / 1000000);\
    }
#endif

#ifndef STATIC_LOG_FORMAT_LOCATION
    #define STATIC_LOG_FORMAT_LOCATION(message, size, level, file, function, line, format, ...) {\
        const char * last_slash = strrchr(file, '/');\
        const char * file_name = last_slash ? last_slash + 1 : file;\
        STATIC_LOG_FORMAT_(snprintf, message, size, "%s:%s:%d", file_name, function, line);\
    }
#endif 

#ifndef STATIC_LOG_FORMAT_MESSAGE
    #define STATIC_LOG_FORMAT_MESSAGE(message, size, level, file, function, line, ...) {\
        STATIC_LOG_FORMAT_PREFIX(message, size, level, file, function, line, __VA_ARGS__);\
        STATIC_LOG_FORMAT_(snprintf, message, size, " ");\
        STATIC_LOG_FORMAT_TIME(message, size, level, file, function, line, __VA_ARGS__);\
        STATIC_LOG_FORMAT_(snprintf, message, size, " ");\
        STATIC_LOG_FORMAT_LOCATION(message, size, level, file, function, line, __VA_ARGS__);\
        STATIC_LOG_FORMAT_(snprintf, message, size, " ");\
        STATIC_LOG_FORMAT_(snprintf, message, size, __VA_ARGS__);\
    }
#endif

#ifndef STATIC_LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_TRACE
#endif

#ifndef STATIC_LOG_BUFFER_SIZE
    #define STATIC_LOG_BUFFER_SIZE 128
#endif

#define STATIC_LOG_START(level, file, function, line, ...) {\
    static int size=0;\
    static char message[STATIC_LOG_BUFFER_SIZE];\
    STATIC_LOG_FORMAT_MESSAGE(message, size, level, file, function, line, __VA_ARGS__);\
    fprintf(stderr, "%s\n", message);\
}
        
#if STATIC_LOG_LEVEL <= STATIC_LOG_LEVEL_FATAL
    #define STATIC_LOG_FATAL(...) STATIC_LOG_START(STATIC_LOG_LEVEL_FATAL, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define STATIC_LOG_FATAL(...) 
#endif 

#if STATIC_LOG_LEVEL <= STATIC_LOG_LEVEL_ERROR
    #define STATIC_LOG_ERROR(...) STATIC_LOG_START(STATIC_LOG_LEVEL_ERROR, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define STATIC_LOG_ERROR(...) 
#endif 

#if STATIC_LOG_LEVEL <= STATIC_LOG_LEVEL_WARN
    #define STATIC_LOG_WARN(...) STATIC_LOG_START(STATIC_LOG_LEVEL_WARN, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define STATIC_LOG_WARN(...) 
#endif 

#if STATIC_LOG_LEVEL <= STATIC_LOG_LEVEL_INFO
    #define STATIC_LOG_INFO(...) STATIC_LOG_START(STATIC_LOG_LEVEL_INFO, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define STATIC_LOG_INFO(...) 
#endif 

#if STATIC_LOG_LEVEL <= STATIC_LOG_LEVEL_DEBUG
    #define STATIC_LOG_DEBUG(...) STATIC_LOG_START(STATIC_LOG_LEVEL_DEBUG, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define STATIC_LOG_DEBUG(...) 
#endif 

#if STATIC_LOG_LEVEL <= STATIC_LOG_LEVEL_TRACE
    #define STATIC_LOG_TRACE(...) STATIC_LOG_START(STATIC_LOG_LEVEL_TRACE, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
    #define STATIC_LOG_TRACE(...) 
#endif 

#ifdef __cplusplus
}
#endif
#endif /* STATIC_LOG_H */
