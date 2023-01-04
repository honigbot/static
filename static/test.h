#ifndef STATIC_TEST_H
#define STATIC_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define _TEST_UNUSED(x) (void)(1+x)

#ifndef TEST_REPORT_XML
    #define TEST_REPORT_XML "report.xml"
#endif

#ifndef TEST_HOSTNAME_LENGTH
    #define TEST_HOSTNAME_LENGTH 256
#endif

#ifndef TEST_MESSAGE_LENGTH
    #define TEST_MESSAGE_LENGTH 512
#endif

const char * _test_assert_name = NULL;
const char * _test_assert_expression = NULL;
const char * _test_assert_file = NULL;
int _test_assert_line = 0;
char _test_assert_message[TEST_MESSAGE_LENGTH];

const char * _test_case_name = NULL;
struct timespec _test_case_timestamp = { 0, 0 }; 
struct timespec _test_case_time = { 0, 0 }; 
int _test_case_error = 0;
int _test_case_failed = 0;
int _test_case_passed = 0;

const char * _test_suite_name = NULL;
struct timespec _test_suite_timestamp = { 0, 0 }; 
struct timespec _test_suite_time = { 0, 0 }; 
int _test_suite_tests = 0;
int _test_suite_errors = 0;
int _test_suite_failures = 0;

struct timespec _test_run_timestamp = { 0, 0 }; 
struct timespec _test_run_time = { 0, 0 };
int _test_run_tests = 0;
int _test_run_failures = 0;
int _test_run_errors = 0;

int _test_xml_case_open_position = 0;
int _test_xml_case_close_position = 0;
int _test_xml_suite_open_position = 0;
int _test_xml_suite_close_position = 0;
int _test_xml_run_open_position = 0;
int _test_xml_run_close_position = 0;
char _test_xml_hostname[TEST_HOSTNAME_LENGTH];
FILE * _test_xml_file = NULL;

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

double _test_time_double(struct timespec time) {
    return time.tv_sec + time.tv_nsec / 1000000000.0;
}

const char * _test_file_name(const char * path) {
    const char * last_slash = strrchr(path, '/');
    return last_slash ? last_slash + 1 : path;
}

// XML

#define _TEST_PREFIX_LENGTH sizeof("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
#define _TEST_TESTS_LENGTH sizeof("tests=\"4294967295\"")
#define _TEST_FAILURES_LENGTH sizeof("failures=\"4294967295\"")
#define _TEST_ERRORS_LENGTH sizeof("errors=\"4294967295\"")
#define _TEST_TIME_LENGTH sizeof("time=\"4294967295.999999999\"")
#define _TEST_TIMESTAMP_LENGTH sizeof("timestamp=\"YYYY-MM-DDThh:mm:ssZ\"")
#define _TEST_HOSTNAME_LENGTH sizeof("hostname=\"\"")
#define _TEST_TESTSUITES_LENGTH (sizeof("<testsuites>") + _TEST_TESTS_LENGTH + _TEST_FAILURES_LENGTH + _TEST_ERRORS_LENGTH + _TEST_TIME_LENGTH)
#define _TEST_TESTSUITE_LENGTH (sizeof("    <testsuite name=\"\">") + _TEST_TESTS_LENGTH + _TEST_FAILURES_LENGTH + _TEST_ERRORS_LENGTH + _TEST_HOSTNAME_LENGTH + _TEST_TIME_LENGTH + _TEST_TIMESTAMP_LENGTH)
#define _TEST_TESTCASE_LENGTH (sizeof("        <testcase name=\"\">") + _TEST_TIME_LENGTH)

void _test_xml_open() {
    gethostname(_test_xml_hostname, sizeof(_test_xml_hostname));
    _test_xml_file = fopen(TEST_REPORT_XML, "w");
    const char * format = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    fprintf(_test_xml_file, "%s", format);
    _test_xml_run_open_position = ftell(_test_xml_file);
}

void _test_xml_close() {
    fclose(_test_xml_file);
}

void _test_xml_run_format_open() {
    const char * format = "<testsuites tests=\"%d\" failures=\"%d\" errors=\"%d\" time=\"%ld.%09ld\">";
    int maximum_length = _TEST_TESTSUITES_LENGTH;
    int written_length = fprintf(_test_xml_file, format, _test_run_tests, _test_run_failures, _test_run_errors, _test_run_time.tv_sec, _test_run_time.tv_nsec);
    fprintf(_test_xml_file, "%*s\n", maximum_length-written_length, "");
}

void _test_xml_run_format_close() {
    fprintf(_test_xml_file, "</testsuites>");
    _TEST_UNUSED(ftruncate(fileno(_test_xml_file), ftell(_test_xml_file)));
}

void _test_xml_run_update() {
    fseek(_test_xml_file, _test_xml_run_open_position, 0);
    _test_xml_run_format_open();
}

void _test_xml_run_open() {
    _test_xml_run_update();
    _test_xml_run_close_position = ftell(_test_xml_file);
}

void _test_xml_run_close() {
    fseek(_test_xml_file, _test_xml_run_close_position, 0);
    _test_xml_run_format_close();
    _test_xml_run_open_position = ftell(_test_xml_file);
}

void _test_xml_suite_format_open() {
    const char * format = "    <testsuite name=\"%s\" tests=\"%d\" failures=\"%d\" errors=\"%d\" hostname=\"%s\" time=\"%ld.%09ld\" timestamp=\"%s\">";
    char timestamp_string[sizeof("YYYY-MM-DDThh:mm:ssZ")];
    strftime(timestamp_string, sizeof(timestamp_string), "%FT%TZ", gmtime(&_test_suite_timestamp.tv_sec));
    int maximum_length = _TEST_TESTSUITE_LENGTH + strlen(_test_suite_name) + strlen(_test_xml_hostname);
    int written_length = fprintf(_test_xml_file, format, _test_suite_name, _test_suite_tests, _test_suite_failures, _test_suite_errors, _test_xml_hostname, _test_suite_time.tv_sec, _test_suite_time.tv_nsec, timestamp_string);
    fprintf(_test_xml_file, "%*s\n", maximum_length-written_length, "");
}

void _test_xml_suite_format_close() {
    fprintf(_test_xml_file, "    </testsuite>\n");
}

void _test_xml_suite_update() {
    fseek(_test_xml_file, _test_xml_suite_open_position, 0);
    _test_xml_suite_format_open();
}

void _test_xml_suite_open() {
    _test_xml_suite_open_position = _test_xml_run_close_position;
    _test_xml_suite_update();
    _test_xml_suite_close_position = ftell(_test_xml_file);
}

void _test_xml_suite_close() {
    fseek(_test_xml_file, _test_xml_suite_close_position, 0);
    _test_xml_suite_format_close();
    _test_xml_run_close_position = ftell(_test_xml_file);
}

void _test_xml_fprintf_xml(const char * string) {
    for(const char * s = string; *s; ) {
        int length = strlen(s);
        int next_replace = strcspn(s, "\"\'<>&");
        fprintf(_test_xml_file, "%.*s", next_replace, s);
        if(next_replace >= length) return;

        switch(s[next_replace]) {
            case '\"': fprintf(_test_xml_file, "&quot;"); break;
            case '\'': fprintf(_test_xml_file, "&apos;"); break;
            case '<':  fprintf(_test_xml_file, "&lt;"); break;
            case '>':  fprintf(_test_xml_file, "&gt;"); break;
            case '&':  fprintf(_test_xml_file, "&amp;"); break;
            default: break;
        }
        s += next_replace+1;
    }
}

void _test_xml_case_format_failure() {
    fprintf(_test_xml_file, "            <failure message=\"%s:%d: ", _test_file_name(_test_assert_file), _test_assert_line);
    _test_xml_fprintf_xml(_test_assert_message);
    fprintf(_test_xml_file, "\" type=\"%s\">\n            </failure>\n", _test_assert_name);
}

void _test_xml_case_format_error() {
    fprintf(_test_xml_file, "%s\n", "            <error message=\"unknown\" type=\"unknown\">\n            </error>");
}

void _test_xml_case_format_open() {
    const char * format = "        <testcase name=\"%s\" time=\"%ld.%09ld\">";
    int maximum_length = _TEST_TESTCASE_LENGTH + strlen(_test_case_name);
    int written_length = fprintf(_test_xml_file, format, _test_case_name, _test_case_time.tv_sec, _test_case_time.tv_nsec);
    fprintf(_test_xml_file, "%*s\n", maximum_length-written_length, "");
}

void _test_xml_case_format_close() {
    fprintf(_test_xml_file, "        </testcase>\n");
}

void _test_xml_case_update() {
    fseek(_test_xml_file, _test_xml_case_open_position, 0);
    _test_xml_case_format_open();
    if(_test_case_error) _test_xml_case_format_error();
    if(_test_case_failed) _test_xml_case_format_failure();
}

void _test_xml_update() {
    _test_xml_run_update();
    _test_xml_suite_update();
    _test_xml_case_update();
    _test_xml_case_close_position = ftell(_test_xml_file);
    _test_xml_case_format_close();
    _test_xml_suite_close_position = ftell(_test_xml_file);
    _test_xml_suite_format_close();
    _test_xml_run_close_position = ftell(_test_xml_file);
    _test_xml_run_format_close();
    fflush(_test_xml_file);
}

void _test_xml_case_open() {
    _test_xml_case_open_position = _test_xml_suite_close_position;
    _test_xml_update();
}

void _test_xml_case_close() {
    _test_xml_update();
}

void _test_print_assert_fail() {
    const char * format = "[\033[1;31mFAIL\033[0m] %s:%s %s:%d: %s %s\033[0m\n";    
    fprintf(stderr, format, _test_suite_name, _test_case_name, _test_file_name(_test_assert_file), _test_assert_line, _test_assert_name, _test_assert_message);
}

void _test_print_case_open() {
    fprintf(stderr, "[\033[1;97mTEST\033[0m] %s:%s\n", _test_suite_name, _test_case_name);
}

void _test_print_case_pass() {
    const char * format = "[\033[1;32mPASS\033[0m] %s:%s (%fs)\n";
    fprintf(stderr, format, _test_suite_name, _test_case_name, _test_time_double(_test_case_time));
}

void _test_print_case_fail() {
    const char * format = "[\033[1;31mFAIL\033[0m] %s:%s (%ld.%09lds)\033[0m\n";
    fprintf(stderr, format, _test_suite_name, _test_case_name, _test_case_time.tv_sec, _test_case_time.tv_nsec);
}

void _test_print_case_close() {
    if(!_test_case_failed) {
        _test_print_case_pass();
    } else {
        // _test_print_case_fail();
    }
}

void _test_print_run_close() {
    const char * format = "[\033[1m====\033[0m] tested: \033[1;95m%d\033[0m passed: \033[1;32m%d\033[0m failed: \033[1;31m%d\033[0m\n";
    fprintf(stderr, format, _test_run_tests, _test_run_tests - _test_run_failures, _test_run_failures);
}

// RUNNER

int _test_new_suite(const char * name) {
    if(_test_suite_name == NULL || name == NULL) return 1; 
    return strcmp(_test_suite_name, name) != 0;
}

int _test_new_run() {
    return _test_run_timestamp.tv_sec == 0 && _test_run_timestamp.tv_nsec == 0;
}

#define _TEST_RUN_OPEN _test_xml_open(); _test_xml_run_open();
#define _TEST_RUN_CLOSE _test_xml_run_close(); _test_xml_close(); _test_print_run_close();

#define _TEST_SUITE_OPEN _test_xml_suite_open();
#define _TEST_SUITE_CLOSE _test_xml_suite_close();

#define _TEST_CASE_OPEN _test_xml_case_open(); _test_print_case_open();
#define _TEST_CASE_CLOSE _test_xml_case_close(); _test_print_case_close();

#define _TEST_ASSERT_PASS 
#define _TEST_ASSERT_FAIL _test_print_assert_fail();

void _test_atexit() {
    _TEST_RUN_CLOSE
}

void _test_case(const char * suite_name, const char * case_name, void (*function)()) {
    if(_test_new_run()) {
        _test_run_timestamp = _test_time_now();
        _test_run_time = _test_time_zero();
        _test_run_failures = 0;
        _test_run_errors = 0;
        _test_run_tests = 0;
        _TEST_RUN_OPEN
        atexit(_test_atexit);
    }

    if(_test_new_suite(suite_name)) {
        if(_test_suite_name != NULL) {
            _TEST_SUITE_CLOSE
        }

        _test_suite_name = suite_name;
        _test_suite_timestamp = _test_time_now(); 
        _test_suite_time = _test_time_zero();
        _test_suite_failures = 0;
        _test_suite_errors = 0;
        _test_suite_tests = 0;
        _TEST_SUITE_OPEN
    }

    _test_case_name = case_name;
    _test_case_timestamp = _test_time_now(); 
    _test_case_time = _test_time_zero();
    _test_case_error = 0;
    _test_case_failed = 0;
    _test_case_passed = 0;

    // register test in suite and run
    _test_suite_tests += 1;
    _test_run_tests += 1;

    // until the test finishes, we assume an error happened
    _test_case_error += 1;
    _test_suite_errors += 1;
    _test_run_errors += 1;

    _TEST_CASE_OPEN

    function();

    // when the test finishes, no error occured
    _test_case_error -= 1;
    _test_suite_errors -= 1;
    _test_run_errors -= 1;

    // register outcome in suite and run
    _test_suite_failures += _test_case_failed > 0 ? 1 : 0;
    _test_run_failures += _test_case_failed > 0 ? 1 : 0;

    // update time tracking
    _test_case_time = _test_time_sub(_test_time_now(), _test_case_timestamp);
    _test_suite_time = _test_time_add(_test_suite_time, _test_case_time);
    _test_run_time = _test_time_add(_test_run_time, _test_case_time);

    _TEST_CASE_CLOSE
}

int _test_assert_evaluate(int result) {
    if(result) {
        _test_case_passed += 1;
        _TEST_ASSERT_PASS
        return 0;
    } else {
        _test_case_failed += 1;
        _TEST_ASSERT_FAIL
        return 1;
    }
}

#define _test_assert_update(_name, _file, _line, _expression, ...) \
    _test_assert_name = _name; \
    _test_assert_file = _file; \
    _test_assert_line = _line; \
    _test_assert_expression = #_expression; \
    snprintf(_test_assert_message, sizeof(_test_assert_message), __VA_ARGS__);

#define _test_assert(_name, _file, _line, _expression, ...) \
    _test_assert_update(_name, _file, _line, _expression, __VA_ARGS__) \
    if(_test_assert_evaluate(_expression)) return;

#define _test_expect(_name, _file, _line, _expression, ...) \
    _test_assert_update(_name, _file, _line, _expression, __VA_ARGS__) \
    _test_assert_evaluate(_expression);

#define test_case(function) _test_case(__func__, #function, function)

// ASSERTIONS 

#define test_assert_message(expression, ...) _test_assert("assert", __FILE__, __LINE__, expression, __VA_ARGS__)
#define test_expect_message(expression, ...) _test_expect("expect", __FILE__, __LINE__, expression, __VA_ARGS__)
#define test_assert(expression) test_assert_message(expression, #expression" (%d)", expression)
#define test_expect(expression) test_expect_message(expression, #expression" (%d)", expression)

#define test_assert_eq_message(a, b, ...) test_assert_message(a == b, __VA_ARGS__)
#define test_assert_ne_message(a, b, ...) test_assert_message(a != b, __VA_ARGS__)
#define test_assert_gt_message(a, b, ...) test_assert_message(a > b, __VA_ARGS__)
#define test_assert_ge_message(a, b, ...) test_assert_message(a >= b, __VA_ARGS__)
#define test_assert_lt_message(a, b, ...) test_assert_message(a < b, __VA_ARGS__)
#define test_assert_le_message(a, b, ...) test_assert_message(a <= b, __VA_ARGS__)

#define test_assert_op_type(a, op, b, format) test_assert_message(a op b, #a" "#op" "#b" ("format" "#op" "format")", a, b)

#define test_assert_eq_int(a, b) test_assert_op_type(a, ==, b, "%d")
#define test_assert_ne_int(a, b) test_assert_op_type(a, !=, b, "%d")
#define test_assert_ge_int(a, b) test_assert_op_type(a, >=, b, "%d")
#define test_assert_le_int(a, b) test_assert_op_type(a, <=, b, "%d")
#define test_assert_gt_int(a, b) test_assert_op_type(a, >, b, "%d")
#define test_assert_lt_int(a, b) test_assert_op_type(a, <, b, "%d")

#define test_assert_eq_uint(a, b) test_assert_op_type(a, ==, b, "%u")
#define test_assert_ne_uint(a, b) test_assert_op_type(a, !=, b, "%u")
#define test_assert_ge_uint(a, b) test_assert_op_type(a, >=, b, "%u")
#define test_assert_le_uint(a, b) test_assert_op_type(a, <=, b, "%u")
#define test_assert_gt_uint(a, b) test_assert_op_type(a, >,  b, "%u")
#define test_assert_lt_uint(a, b) test_assert_op_type(a, <,  b, "%u")

#define test_assert_eq_float(a, b) test_assert_op_type(a, ==, b, "%f")
#define test_assert_ne_float(a, b) test_assert_op_type(a, !=, b, "%f")
#define test_assert_ge_float(a, b) test_assert_op_type(a, >=, b, "%f")
#define test_assert_le_float(a, b) test_assert_op_type(a, <=, b, "%f")
#define test_assert_gt_float(a, b) test_assert_op_type(a, >,  b, "%f")
#define test_assert_lt_float(a, b) test_assert_op_type(a, <,  b, "%f")

#define test_assert_eq_hex(a, b) test_assert_op_type(a, ==, b, "%x")
#define test_assert_ne_hex(a, b) test_assert_op_type(a, !=, b, "%x")
#define test_assert_ge_hex(a, b) test_assert_op_type(a, >=, b, "%x")
#define test_assert_le_hex(a, b) test_assert_op_type(a, <=, b, "%x")
#define test_assert_gt_hex(a, b) test_assert_op_type(a, >,  b, "%x")
#define test_assert_lt_hex(a, b) test_assert_op_type(a, <,  b, "%x")

#define test_assert_eq_char(a, b) test_assert_op_type(a, ==, b, "%c")
#define test_assert_ne_char(a, b) test_assert_op_type(a, !=, b, "%c")
#define test_assert_gt_char(a, b) test_assert_op_type(a, >=, b, "%c")
#define test_assert_ge_char(a, b) test_assert_op_type(a, <=, b, "%c")
#define test_assert_lt_char(a, b) test_assert_op_type(a, >,  b, "%c")
#define test_assert_le_char(a, b) test_assert_op_type(a, <,  b, "%c")

#define test_assert_eq_string(a, b) test_assert_message(strcmp(a, b) == 0, "\"%s\" == \"%s\"", a, b)
#define test_assert_ne_string(a, b) test_assert_message(strcmp(a, b) != 0, "\"%s\" != \"%s\"", a, b)

#define test_assert_eq_bytes(a, b, size) test_assert_message(memcmp(a, b) == 0, "\"%s\" == \"%s\"", a, b)
#define test_assert_ne_bytes(a, b, size) test_assert_message(strcmp(a, b) == 0, "\"%s\" == \"%s\"", a, b)

#endif /* STATIC_TEST_H */
