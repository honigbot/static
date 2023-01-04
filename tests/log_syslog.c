#include <syslog.h>
#include <unistd.h>

int main() {
    // setlogmask (LOG_UPTO (LOG_NOTICE));

    openlog ("exampleprog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);

    for (int pri = 0 ; pri < 8 ; pri++)
        syslog(LOG_ERR,"This is a test %d", pri);

    // syslog (LOG_ERR, "Program started by User %d", getuid());
    // syslog (LOG_NOTICE, "A tree falls in a forest");
    // syslog (LOG_DEBUG, "A tree falls in a forest");
// 
    // closelog ();
}
