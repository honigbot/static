#include <os/log.h>

os_log_t *_log_logger;

#define OS_LOG_TARGET OS_LOG_DEFAULT
#define LOG_OSLOG_SUBSYSTEM ""          // "com.example.subsystem"
#define LOG_OSLOG_CATEGORY "static/log" // "category"

os_log_t * get_logger() {
    static int once = 1;
    if (once) {
        once = 0;
        static os_log_t logger;
        logger = os_log_create(LOG_OSLOG_SUBSYSTEM, LOG_OSLOG_CATEGORY);
        _log_logger = &logger;
    }
    return _log_logger;
}

#define LOG_SYSTEM 1
#define LOG_SYSTEM_ID "com.example.subsystem"
#define LOG_SYSTEM_CATERGORY "static/log"

// initialize_log(argc, argv);
// initialize_test(argc, argv);

// TRACE -> os_log_debug
// DEBUG -> os_log_debug
// INFO -> os_log_info
// WARNING -> os_log_error
// ERROR -> os_log_error
// FATAL -> os_log_fault

int main() {
    os_log_t logger = *get_logger();

    os_log_debug(logger, "My debug message %d", 0); // OS_LOG_TYPE_DEBUG
    os_log_info(logger, "My info message %d", 1);   // OS_LOG_TYPE_INFO
    os_log(logger, "My log message %d", 2);         // OS_LOG_TYPE_DEFAULT // WARN
    os_log_error(logger, "My error message %d", 3); // OS_LOG_TYPE_ERROR
    os_log_fault(logger, "My log message %d", 4);   // OS_LOG_TYPE_ERROR

    // os_log_info(OS_LOG_TARGET, "My log message %{private}@", someString); //
    // OS_LOG_TYPE_INFO os_log(OS_LOG_TARGET, "My log message %{private}@",
    // someString); // OS_LOG_TYPE_DEFAULT os_log_fault(OS_LOG_TARGET, "My error
    // log message %{public}@", someString); // OS_LOG_TYPE_FAULT

    return 0;
}
