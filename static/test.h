#ifndef STATIC_TEST_H
#define STATIC_TEST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

struct {
    const char * name;
    struct timespec start;
    struct timespec end;
    int failed;
    int passed;
    int status;
    int assertions;
    int position;
} _test_case;

struct {
    const char * name;
    struct timespec start;
    struct timespec end;
    int tests;
    int failures;
    int disabled;
    int skipped;
    int errors;
    int position;
} _test_suite;

struct {
    const char * name;
    struct timespec start;
    struct timespec end;
    int tests;
    int failures;
    FILE * file;
} _test_run;

struct timespec _test_time_now() {
    struct timespec t;    
    clock_gettime(CLOCK_REALTIME, &t);
    return t;
}

struct timespec _test_time_delta(const struct timespec * start, const struct timespec * stop) {
    struct timespec delta;
    delta.tv_sec = stop->tv_sec - start->tv_sec;
    delta.tv_nsec = stop->tv_nsec - start->tv_nsec;
    if(delta.tv_nsec < 0) {
        delta.tv_sec -= 1;
        delta.tv_nsec = 1000000000 + delta.tv_nsec;
    }
    return delta;
}

void _test_empty() {}
#define TEST_MAX_HOSTNAME_LENGTH 256

// REPORT

#define _TEST_PREFIX_LENGTH sizeof("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
#define _TEST_TESTS_LENGTH sizeof("tests=\"4294967295\"")
#define _TEST_FAILURES_LENGTH sizeof("failures=\"4294967295\"")
#define _TEST_TIME_LENGTH sizeof("time=\"4294967295.999999999\"")
#define _TEST_TIMESTAMP_LENGTH sizeof("timestamp=\"YYYY-MM-DDThh:mm:ssZ\"")
#define _TEST_HOSTNAME_LENGTH sizeof("hostname=\"\"") + TEST_MAX_HOSTNAME_LENGTH
#define _TEST_TESTSUITES_LENGTH (sizeof("<testsuites name=\"\">") + _TEST_TESTS_LENGTH + _TEST_FAILURES_LENGTH + _TEST_TIME_LENGTH)
#define _TEST_TESTSUITE_LENGTH (sizeof("    <testsuite name=\"\">") + _TEST_TESTS_LENGTH + _TEST_FAILURES_LENGTH + _TEST_HOSTNAME_LENGTH + _TEST_TIME_LENGTH + _TEST_TIMESTAMP_LENGTH)
#define _TEST_TESTCASE_LENGTH (sizeof("        <testcase name=\"\">") + _TEST_TIME_LENGTH)

void _test_report_run_format() {
    const char * format = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<testsuites name=\"%s\" tests=\"%d\" failures=\"%d\" time=\"%d.%09d\">";
    struct timespec time = _test_time_delta(&_test_run.start, &_test_run.end);
    int maximum_length = _TEST_PREFIX_LENGTH + _TEST_TESTSUITES_LENGTH + strlen(_test_run.name);
    int written_length = fprintf(_test_run.file, format, _test_run.name, _test_run.tests, _test_run.failures, time.tv_sec, time.tv_nsec);
    fprintf(_test_run.file, "%*s\n", maximum_length-written_length, "");
}

void _test_report_run_open() {
    _test_run.file = fopen("report.xml", "w");
    _test_report_run_format();
}

void _test_report_run_close() {
    int current_position = ftell(_test_run.file);
    fseek(_test_run.file, 0, 0);
    _test_report_run_format();
    fseek(_test_run.file, current_position, 0);
    fprintf(_test_run.file, "</testsuites>");
    fclose(_test_run.file);
}

void _test_report_suite_format() {
    const char * format = "    <testsuite name=\"%s\" tests=\"%d\" failures=\"%d\" hostname=\"%s\" time=\"%d.%09d\" timestamp=\"%s\">";
    struct timespec time = _test_time_delta(&_test_suite.start, &_test_suite.end);
    char timestamp_string[sizeof("YYYY-MM-DDThh:mm:ssZ")];
	strftime(timestamp_string, sizeof(timestamp_string), "%FT%TZ", gmtime(&_test_suite.start.tv_sec));
    char hostname_string[TEST_MAX_HOSTNAME_LENGTH];
    gethostname(hostname_string,  sizeof(hostname_string));
	int maximum_length = _TEST_TESTSUITE_LENGTH + strlen(_test_suite.name);
	int written_length = fprintf(_test_run.file, format, _test_suite.name, _test_suite.tests, _test_suite.failures, hostname_string, time.tv_sec, time.tv_nsec, timestamp_string);
	fprintf(_test_run.file, "%*s\n", maximum_length-written_length, "");
}

void _test_report_suite_open() {
    _test_suite.position = ftell(_test_run.file);
	_test_report_suite_format();
}

void _test_report_suite_close() {
    int current_position = ftell(_test_run.file);
    fseek(_test_run.file, _test_suite.position, 0);
    _test_report_suite_format();
    fseek(_test_run.file, current_position, 0);
    fprintf(_test_run.file, "    </testsuite>\n");
}

void _test_report_case_format() {
    const char * format = "        <testcase name=\"%s\" time=\"%d.%09d\">";
    struct timespec time = _test_time_delta(&_test_case.start, &_test_case.end);
	int maximum_length = _TEST_TESTCASE_LENGTH + strlen(_test_case.name);
	int written_length = fprintf(_test_run.file, format, _test_case.name, time.tv_sec, time.tv_nsec);
	fprintf(_test_run.file, "%*s\n", maximum_length-written_length, "");
}

void _test_report_case_open() {
    _test_case.position = ftell(_test_run.file);
	_test_report_case_format();
}

void _test_report_case_close() {
    int current_position = ftell(_test_run.file);
	fseek(_test_run.file, _test_case.position, 0);
    _test_report_case_format();
	fseek(_test_run.file, current_position, 0);
	fprintf(_test_run.file, "        </testcase>\n");
}

#define _TEST_REPORT_ASSERT_FAIL(_name, _file, _line, _expression, ...) \
	fprintf(_test_run.file, "            <failure message=\""); \
	fprintf(_test_run.file, " "__VA_ARGS__); \
	fprintf(_test_run.file, "\" type=\"%s\">%s:%d:    %s</failure>\n", _name, _file, _line, _expression);

#define _TEST_REPORT_ASSERT_PASS(_name, _file, _line, _expression, ...)

// PRINT

#define _TEST_PRINT_ASSERT_PASS(_name, _file, _line, _expression, ...) \
    fprintf(stderr, "\033[1;32m‣ %s:%s:%s:%s(%s)", _test_run.name, _test_suite.name, _test_case.name, _name, _expression); \
    fprintf(stderr, " "__VA_ARGS__); \
	fprintf(stderr, "\033[0m\n") \

#define _TEST_PRINT_ASSERT_FAIL(_name, _file, _line, _expression, ...) { \
    fprintf(stderr, "\033[1;31m‣ %s:%d:%s: %s", _file, _line, _name, _expression); \
	fprintf(stderr, " "__VA_ARGS__); \
	fprintf(stderr, "\033[0m\n"); \
}

void _test_print_case_open() {
    fprintf(stderr, "\033[1m‣ %s:%s:%s\033[0m\n", _test_run.name, _test_suite.name, _test_case.name);
}

void _test_print_case_close() {
    fprintf(stderr, "\n");
}

void _test_print_case_pass() {
    struct timespec __time = _test_time_delta(&_test_case.start, &_test_case.end); \
	fprintf(stderr, "\033[1;32m‣ %s:%s:%s (%ld.%09lds)\033[0m\n", _test_run.name, _test_suite.name, _test_case.name, __time.tv_sec, __time.tv_nsec);\
}

void _test_print_case_fail() {
    struct timespec __time = _test_time_delta(&_test_case.start, &_test_case.end); \
	fprintf(stderr, "\033[1;31m‣ %s:%s:%s (%ld.%09lds)\033[0m\n", _test_run.name, _test_suite.name, _test_case.name, __time.tv_sec, __time.tv_nsec);\
}


// CUSTOMIZATION

#define _TEST_CASE_OPEN _test_report_case_open(); _test_print_case_open();
#define _TEST_CASE_CLOSE _test_report_case_close(); _test_print_case_close();
#define _TEST_CASE_PASS _test_print_case_pass();
#define _TEST_CASE_FAIL _test_print_case_fail();
#define _TEST_SUITE_OPEN _test_report_suite_open();
#define _TEST_SUITE_CLOSE _test_report_suite_close();
#define _TEST_RUN_OPEN _test_report_run_open();
#define _TEST_RUN_CLOSE _test_report_run_close();
#define TEST_ASSERT_PASS(_name, _file, _line, _expression, ...) \
    _TEST_REPORT_ASSERT_PASS(_name, _file, _line, _expression, __VA_ARGS__);
#define TEST_ASSERT_FAIL(_name, _file, _line, _expression, ...) \
    _TEST_PRINT_ASSERT_FAIL(_name, _file, _line, _expression, __VA_ARGS__); \
    _TEST_REPORT_ASSERT_FAIL(_name, _file, _line, _expression, __VA_ARGS__); 

void _test_case_(const char * name, void(*function)()) {
    _test_case.name = name; 
    _test_case.start = _test_time_now(); 
    _test_case.end = _test_case.start;
    _test_case.failed = 0; 
    _test_case.passed = 0;
    _test_case.position = 0;

    _TEST_CASE_OPEN

    function();
    _test_case.end = _test_time_now();

    if(!_test_case.failed) {
        _TEST_CASE_PASS
    } else {
        _TEST_CASE_FAIL
    }

    _TEST_CASE_CLOSE

    _test_suite.tests += 1;
    _test_suite.failures += _test_case.failed;
}

void _test_suite_(const char * name, void(*function)()) {
    _test_suite.name = name; 
    _test_suite.start = _test_time_now(); 
    _test_suite.end = _test_suite.start;
    _test_suite.failures = 0;
    _test_suite.position = 0;
    _test_suite.tests = 0;

    _TEST_SUITE_OPEN

    function();
    _test_suite.end = _test_time_now();

    _TEST_SUITE_CLOSE

    _test_run.tests += _test_suite.tests;
    _test_run.failures += _test_suite.failures;
}

int _test_run_(const char * name, void (*function)()) {
    _test_run.name = name;
    _test_run.start = _test_time_now();
    _test_run.end = _test_run.start;
    _test_run.failures = 0;
    _test_run.tests = 0;

    _TEST_RUN_OPEN

    function();
    _test_run.end = _test_time_now();

    _TEST_RUN_CLOSE

    return _test_run.failures > 0;
}

#define _TEST_ASSERT(_name, _file, _line, _expression, ...) \
	if((_expression)) { \
        _test_case.passed += 1; \
        TEST_ASSERT_PASS(_name, _file, _line, #_expression, __VA_ARGS__); \
	} else { \
        _test_case.failed += 1; \
        TEST_ASSERT_FAIL(_name, _file, _line, #_expression, __VA_ARGS__); \
        return; \
    }

#define TEST_CASE(function) _test_case_(#function, function)
#define TEST_SUITE(function) _test_suite_(#function, function)
#define TEST_RUN(function) _test_run_(#function, function);

#define TEST_ASSERT_MESSAGE(expression, ...) _TEST_ASSERT("TEST_ASSERT_MESSAGE", __FILE__, __LINE__, expression, __VA_ARGS__)
#define TEST_ASSERT(expression) _TEST_ASSERT("TEST_ASSERT", __FILE__, __LINE__, expression, "")

#endif /* STATIC_TEST_H */
