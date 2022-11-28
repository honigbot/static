#ifndef STATIC_TEST_H
#define STATIC_TEST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define __USE_POSIX199309
#include <time.h>
#define _TEST_UNUSED(x) (void)(x)

#ifndef TEST_REPORT
    #define TEST_REPORT "report.xml"
#endif

#ifndef TEST_HOSTNAME_LENGTH
    #define TEST_HOSTNAME_LENGTH 256
#endif

#ifndef TEST_MESSAGE_LENGTH
    #define TEST_MESSAGE_LENGTH 512
#endif

struct {
    const char * name;
    const char * expression;
    const char * file;
    int line;
    char message[TEST_MESSAGE_LENGTH];
} _test_assertion;

struct {
    const char * name;
    struct timespec timestamp;
    struct timespec time;
    int failed;
    int error;
    int passed;
    int status;
} _test_case;

struct {
    const char * name;
    struct timespec timestamp;
    struct timespec time;
    int tests;
    int failures;
    int errors;
} _test_suite;

struct {
    const char * name;
    struct timespec timestamp;
    struct timespec time;
    int tests;
    int failures;
    int errors;
} _test_run;

struct {
    int case_open;
    int case_close;
    int suite_open;
    int suite_close;
    int run_open;
    int run_close;
    FILE * file;
} _test_report;

struct timespec _test_time_now() {
    struct timespec t;    
    clock_gettime(CLOCK_REALTIME, &t);
    return t;
}

struct timespec _test_time_sub(struct timespec a, struct timespec b) {
    struct timespec sub;
    sub.tv_sec = a.tv_sec - b.tv_sec;
    sub.tv_nsec = a.tv_nsec - b.tv_nsec;
    if(sub.tv_nsec < 0) {
        sub.tv_sec -= 1;
        sub.tv_nsec += 1000000000;
    }
    return sub;
}

struct timespec _test_time_add(struct timespec a, struct timespec b) {
    struct timespec add;
    add.tv_sec = a.tv_sec + b.tv_sec;
    add.tv_nsec = a.tv_nsec + b.tv_nsec;
    if(add.tv_nsec >= 1000000000) {
        add.tv_sec += 1;
        add.tv_nsec -= 1000000000;
    }
    return add;
}

struct timespec _test_time_zero() {
    struct timespec zero;
    zero.tv_sec = 0;
    zero.tv_nsec = 0;
    return zero;
}

// REPORT

#define _TEST_PREFIX_LENGTH sizeof("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
#define _TEST_TESTS_LENGTH sizeof("tests=\"4294967295\"")
#define _TEST_FAILURES_LENGTH sizeof("failures=\"4294967295\"")
#define _TEST_ERRORS_LENGTH sizeof("errors=\"4294967295\"")
#define _TEST_TIME_LENGTH sizeof("time=\"4294967295.999999999\"")
#define _TEST_TIMESTAMP_LENGTH sizeof("timestamp=\"YYYY-MM-DDThh:mm:ssZ\"")
#define _TEST_HOSTNAME_LENGTH sizeof("hostname=\"\"") + TEST_HOSTNAME_LENGTH
#define _TEST_TESTSUITES_LENGTH (sizeof("<testsuites name=\"\">") + _TEST_TESTS_LENGTH + _TEST_FAILURES_LENGTH + _TEST_ERRORS_LENGTH + _TEST_TIME_LENGTH)
#define _TEST_TESTSUITE_LENGTH (sizeof("    <testsuite name=\"\">") + _TEST_TESTS_LENGTH + _TEST_FAILURES_LENGTH + _TEST_ERRORS_LENGTH + _TEST_HOSTNAME_LENGTH + _TEST_TIME_LENGTH + _TEST_TIMESTAMP_LENGTH)
#define _TEST_TESTCASE_LENGTH (sizeof("        <testcase name=\"\">") + _TEST_TIME_LENGTH)

void _test_report_format() {
    const char * format = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    fprintf(_test_report.file, "%s", format);
}

void _test_report_open() {
    _test_report.file = fopen(TEST_REPORT, "w");
    _test_report_format();
    _test_report.run_open = ftell(_test_report.file);
}

void _test_report_close() {
    fclose(_test_report.file);
}

void _test_report_run_format_open() {
    const char * format = "<testsuites name=\"%s\" tests=\"%d\" failures=\"%d\" errors=\"%d\" time=\"%ld.%09ld\">";
    int maximum_length = _TEST_PREFIX_LENGTH + _TEST_TESTSUITES_LENGTH + strlen(_test_run.name);
    int written_length = fprintf(_test_report.file, format, _test_run.name, _test_run.tests, _test_run.failures, _test_run.errors, _test_run.time.tv_sec, _test_run.time.tv_nsec);
    fprintf(_test_report.file, "%*s\n", maximum_length-written_length, "");
}

void _test_report_run_format_close() {
    fprintf(_test_report.file, "</testsuites>");
}

void _test_report_run_update() {
    fseek(_test_report.file, _test_report.run_open, 0);
    _test_report_run_format_open();
}

void _test_report_run_open() {
    _test_report_run_update();
    _test_report.run_close = ftell(_test_report.file);
}

void _test_report_run_close() {
    fseek(_test_report.file, _test_report.run_close, 0);
    _test_report_run_format_close();
    _test_report.run_open = ftell(_test_report.file);
}

void _test_report_suite_format_open() {
    const char * format = "    <testsuite name=\"%s\" tests=\"%d\" failures=\"%d\" errors=\"%d\" hostname=\"%s\" time=\"%ld.%09ld\" timestamp=\"%s\">";
    char timestamp_string[sizeof("YYYY-MM-DDThh:mm:ssZ")];
    strftime(timestamp_string, sizeof(timestamp_string), "%FT%TZ", gmtime(&_test_suite.timestamp.tv_sec));
    char hostname_string[TEST_HOSTNAME_LENGTH];
    gethostname(hostname_string,  sizeof(hostname_string));
    int maximum_length = _TEST_TESTSUITE_LENGTH + strlen(_test_suite.name);
    int written_length = fprintf(_test_report.file, format, _test_suite.name, _test_suite.tests, _test_suite.failures, _test_suite.errors, hostname_string, _test_suite.time.tv_sec, _test_suite.time.tv_nsec, timestamp_string);
    fprintf(_test_report.file, "%*s\n", maximum_length-written_length, "");
}

void _test_report_suite_format_close() {
    fprintf(_test_report.file, "    </testsuite>\n");
}

void _test_report_suite_update() {
    fseek(_test_report.file, _test_report.suite_open, 0);
    _test_report_suite_format_open();
}

void _test_report_suite_open() {
    _test_report.suite_open = _test_report.run_close;
    _test_report_suite_update();
    _test_report.suite_close = ftell(_test_report.file);
}

void _test_report_suite_close() {
    fseek(_test_report.file, _test_report.suite_close, 0);
    _test_report_suite_format_close();
    _test_report.run_close = ftell(_test_report.file);
}

void _test_report_fprintf_xml(const char * string) {
    for(const char * s = string; *s; ) {
        int length = strlen(s);
        int next_replace = strcspn(s, "\"\'<>&");
        fprintf(_test_report.file, "%.*s", next_replace, s);
        if(next_replace >= length) return;

        switch(s[next_replace]) {
            case '\"': fprintf(_test_report.file, "&quot;"); break;
            case '\'': fprintf(_test_report.file, "&apos;"); break;
            case '<': fprintf(_test_report.file, "&lt;"); break;
            case '>': fprintf(_test_report.file, "&gt;"); break;
            case '&': fprintf(_test_report.file, "&amp;"); break;
            default: break;
        }
        s += next_replace+1;
    }
}

void _test_report_case_format_failure() {
    fprintf(_test_report.file, "            <failure message=\"%s:%d: ", _test_assertion.file, _test_assertion.line);
    _test_report_fprintf_xml(_test_assertion.expression); 
    fprintf(_test_report.file, " ");
    _test_report_fprintf_xml(_test_assertion.message);
    fprintf(_test_report.file, "\" type=\"%s\">\n            </failure>\n", _test_assertion.name);
}

void _test_report_case_format_error() {
    fprintf(_test_report.file, "%s\n", "            <error message=\"unknown\" type=\"unknown\">\n            </error>");
}

void _test_report_case_format_open() {
    const char * format = "        <testcase name=\"%s\" time=\"%ld.%09ld\">";
    int maximum_length = _TEST_TESTCASE_LENGTH + strlen(_test_case.name);
    int written_length = fprintf(_test_report.file, format, _test_case.name, _test_case.time.tv_sec, _test_case.time.tv_nsec);
    fprintf(_test_report.file, "%*s\n", maximum_length-written_length, "");
}

void _test_report_case_format_close() {
    fprintf(_test_report.file, "        </testcase>\n");
}

void _test_report_case_update() {
    fseek(_test_report.file, _test_report.case_open, 0);
    _test_report_case_format_open();
    if(_test_case.error) _test_report_case_format_error();
    if(_test_case.failed) _test_report_case_format_failure();
}

void _test_report_update() {
    _test_report_run_update();
    _test_report_suite_update();
    _test_report_case_update();
    _test_report.case_close = ftell(_test_report.file);
    _test_report_case_format_close();
    _test_report.suite_close = ftell(_test_report.file);
    _test_report_suite_format_close();
    _test_report.run_close = ftell(_test_report.file);
    _test_report_run_format_close();
}

void _test_report_case_open() {
    _test_report.case_open = _test_report.suite_close;
    _test_report_update();
}

void _test_report_case_close() {
    _test_report_update();
}

// PRINT

void _test_print_assert_pass() {
    const char * format = "\033[1;32m‣ %s:%s:%s:%s(%s) %s\033[0m\n";
    fprintf(stderr, format, _test_run.name, _test_suite.name, _test_case.name, _test_assertion.name, _test_assertion.expression, _test_assertion.message);
}

void _test_print_assert_fail() {
    const char * format = "\033[1;31m‣ %s:%d:%s: %s %s\033[0m\n";
    fprintf(stderr, format, _test_assertion.file, _test_assertion.line, _test_assertion.name, _test_assertion.expression, _test_assertion.message);
}

void _test_print_case_open() {
    fprintf(stderr, "\033[1m‣ %s:%s:%s\033[0m\n", _test_run.name, _test_suite.name, _test_case.name);
}

void _test_print_case_pass() {
    const char * format = "\033[1;32m‣ %s:%s:%s (%ld.%09lds)\033[0m\n";
    fprintf(stderr, format, _test_run.name, _test_suite.name, _test_case.name, _test_case.time.tv_sec, _test_case.time.tv_nsec);
}

void _test_print_case_fail() {
    const char * format = "\033[1;31m‣ %s:%s:%s (%ld.%09lds)\033[0m\n";
    fprintf(stderr, format, _test_run.name, _test_suite.name, _test_case.name, _test_case.time.tv_sec, _test_case.time.tv_nsec);
}

void _test_print_case_close() {
    if(!_test_case.failed) {
        _test_print_case_pass();
    } else {
        _test_print_case_fail();
    }
    fprintf(stderr, "\n");
}


// CUSTOMIZATION

#define _TEST_CASE_OPEN _test_report_case_open(); _test_print_case_open();
#define _TEST_CASE_CLOSE _test_report_case_close(); _test_print_case_close();
#define _TEST_SUITE_OPEN _test_report_suite_open();
#define _TEST_SUITE_CLOSE _test_report_suite_close();
#define _TEST_RUN_OPEN _test_report_open(); _test_report_run_open();
#define _TEST_RUN_CLOSE _test_report_run_close(); _test_report_close();
#define _TEST_ASSERT_PASS 
#define _TEST_ASSERT_FAIL _test_print_assert_fail();

void _test_case_(const char * name, void(*function)()) {
    _test_case.name = name; 
    _test_case.timestamp = _test_time_now(); 
    _test_case.time = _test_time_zero();
    _test_case.error = 1;
    _test_case.failed = 0; 
    _test_case.passed = 0;

    _test_suite.tests += 1;
    _test_suite.errors += 1;

    _test_run.tests += 1;
    _test_run.errors += 1;

    _TEST_CASE_OPEN

    function();

    _test_case.error = 0;
    _test_case.time = _test_time_sub(_test_time_now(), _test_case.timestamp);

    _test_suite.time = _test_time_add(_test_suite.time, _test_case.time);
    _test_suite.errors -= 1;
    _test_suite.failures += _test_case.failed;

    _test_run.time = _test_time_add(_test_run.time, _test_case.time);
    _test_run.errors -= 1;
    _test_run.failures += _test_case.failed;

    _TEST_CASE_CLOSE
}

void _test_suite_(const char * name, void(*function)()) {
    _test_suite.name = name; 
    _test_suite.timestamp = _test_time_now(); 
    _test_suite.time = _test_time_zero();
    _test_suite.failures = 0;
    _test_suite.errors = 0;
    _test_suite.tests = 0;

    _TEST_SUITE_OPEN
    function();
    _TEST_SUITE_CLOSE
}

int _test_run_(const char * name, void (*function)(), int argc, char ** argv) {
    _TEST_UNUSED(argc);
    _TEST_UNUSED(argv);
    _test_run.name = name;
    _test_run.timestamp = _test_time_now();
    _test_run.time = _test_time_zero();
    _test_run.failures = 0;
    _test_run.errors = 0;
    _test_run.tests = 0;

    _TEST_RUN_OPEN
    function();
    _TEST_RUN_CLOSE

    return 0;
}

void _test_assert(int result) {
    if(result) {
        _test_case.passed += 1;
        _TEST_ASSERT_PASS
    } else {
        _test_case.failed += 1;
        _TEST_ASSERT_FAIL
        return;
    }
}

#define _TEST_ASSERT(_name, _file, _line, _expression, ...) \
    _test_assertion.name = _name; \
    _test_assertion.file = _file; \
    _test_assertion.line = _line;  \
    _test_assertion.expression = #_expression; \
    snprintf(_test_assertion.message, sizeof(_test_assertion.message), __VA_ARGS__); \
    _test_assert(_expression);

#define TEST_CASE(function) _test_case_(#function, function)
#define TEST_SUITE(function) _test_suite_(#function, function)
#define TEST_RUN(function, argc, argv) _test_run_(#function, function, argc, argv);

#define TEST_ASSERT_MESSAGE(expression, ...) _TEST_ASSERT("TEST_ASSERT_MESSAGE", __FILE__, __LINE__, expression, __VA_ARGS__)
#define TEST_ASSERT(expression) _TEST_ASSERT("TEST_ASSERT", __FILE__, __LINE__, expression, " ")

#endif /* STATIC_TEST_H */
