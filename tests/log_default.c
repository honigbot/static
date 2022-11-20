#include "static/log.h"

int main() {
    STATIC_LOG_TRACE("trace %d", 1);
    STATIC_LOG_DEBUG("debug %d", 2);
    STATIC_LOG_INFO("info %d", 3);
    STATIC_LOG_WARN("warn %d", 4);
    STATIC_LOG_ERROR("error %d", 5);
    STATIC_LOG_FATAL("fatal %d", 6);
    return 0;
}