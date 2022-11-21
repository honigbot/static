#ifndef STATIC_TEST_H
#define STATIC_TEST_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

typedef struct test_assertion_t {
    const char * name;
    const char * file;
    const char * expression;
    int line;
} test_assertion_t;

typedef struct test_case_t {
    const char * name;
	double time;
    void (*function)();
} test_case_t;

typedef struct test_suite_t {
    const char * name;
    FILE * file;
} test_suite_t;

test_suite_t __test_suite;
test_case_t __test_case;
test_assertion_t __test_assertion;

void report_test_suite_open() {
    #ifdef STATIC_TEST_REPORT
    __test_suite.file = fopen("report.xml", "w");
    fprintf(__test_suite.file, "<testsuite name=\"%s\">\n", __test_suite.name);
    #endif
}

void report_test_suite_close() {
    #ifdef STATIC_TEST_REPORT
    fprintf(__test_suite.file, "</testsuite>");
    fclose(__test_suite.file);
    #endif
}

void report_test_case_failure() {
    #ifdef STATIC_TEST_REPORT
    fprintf(
        __test_suite.file, 
        "    <testcase name=\"%s()\" time=\"%f\">\n"
        "        <failure type=\"%s\">%s:%d:    %s</failure>\n"
        "    </testcase>\n",
        __test_case.name, 
		__test_case.time,
        __test_assertion.name,
        __test_assertion.file, 
        __test_assertion.line, 
        __test_assertion.expression
    );
    #endif
}

void report_test_case_success() {
    #ifdef STATIC_TEST_REPORT
    fprintf(
		__test_suite.file, 
        "    <testcase name=\"%s()\" time=\"%f\"></testcase>\n",
        __test_case.name,
		__test_case.time
    );
    #endif
}

void static_test_suite(const char * name, void (*function)()) {
    __test_suite.name = name;
    report_test_suite_open();
    function();
    report_test_suite_close();
}

double static_test_time() {
	struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
	return (double)time.tv_sec + 1.0e-9*time.tv_nsec;
}

void static_test_case(const char * name, void (*function)()) {
    __test_case.name = name;
    __test_assertion.file = NULL;

    fprintf(stderr, "\033[1m⬜ EXECUTING %s()\033[0m\n", __test_case.name );

	double start = static_test_time();
	
	function();

	double end = static_test_time();
	__test_case.time = end - start;

	if(__test_assertion.file == NULL) {
        fprintf(stderr, "\033[1;32m✅ PASSED %s()\033[0m\n", __test_case.name);
        report_test_case_success();
	} else {
        fprintf(stderr, "\033[1;31m⛔ FAILED %s():\033[0m %s:%d: \n    %s\n", __test_case.name, __test_assertion.file, __test_assertion.line, __test_assertion.expression);
        report_test_case_failure();
	}
}

void static_test_assert(const char * name, const char * file, int line, const char * expression) {
    __test_assertion.name = name;
	__test_assertion.file = file;
	__test_assertion.line = line;
	__test_assertion.expression = expression;
}

#define STATIC_TEST_SUITE(test_suite_name) static_test_suite(#test_suite_name, test_suite_name)
#define STATIC_TEST_CASE(function) static_test_case(#function, function)

#define STATIC_ASSERT_(name, expression) if(!(expression)) { static_test_assert(name, __FILE__, __LINE__, #expression); return; }
#define STATIC_ASSERT(expression) STATIC_ASSERT_("ASSERT", expression)
#define STATIC_ASSERT_EQ(expected, actual) STATIC_ASSERT_("ASSERT_EQ", expected == actual)
#define STATIC_ASSERT_NE(expected, actual) STATIC_ASSERT_("ASSERT_NE", expected != actual)
#define STATIC_ASSERT_GE(expected, actual) STATIC_ASSERT_("ASSERT_GE", expected >= actual)
#define STATIC_ASSERT_LE(expected, actual) STATIC_ASSERT_("ASSERT_LE", expected <= actual)
#define STATIC_ASSERT_GT(expected, actual) STATIC_ASSERT_("ASSERT_GT", expected > actual)
#define STATIC_ASSERT_LT(expected, actual) STATIC_ASSERT_("ASSERT_LT", expected < actual)

#endif /* STATIC_TEST_H */
