#define LOG_FORMAT_PREFIX(message, size, level, file, function, line, format, ...) {\
    static const char * prefix[] = { "👣", "🐞", "💬", "⚠️ ", "💥", "💀" };\
    LOG_FORMAT_(snprintf, message, size, "%s", prefix[level]);\
}

#include "static/log.h"

int main() {
    LOG_TRACE("trace %d", 1);
    LOG_DEBUG("debug %d", 2);
    LOG_INFO("info %d", 3);
    LOG_WARN("warn %d", 4);
    LOG_ERROR("error %d", 5);
    LOG_FATAL("fatal %d", 6);
    return 0;
}
