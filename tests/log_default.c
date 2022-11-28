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
