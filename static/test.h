#ifndef STATIC_TEST_H
#define STATIC_TEST_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

typedef struct test_assertion_t {
    const char * name;
    const char * file;
    const char * expression;
	const char * message;
    int line;
} test_assertion_t;

typedef struct test_case_t {
    const char * name;
	double time;
    void (*function)();
} test_case_t;

typedef struct test_suite_t {
    const char * name;
	double time;
	time_t timestamp;
	uint32_t tests;
	uint32_t failures;
	int start;
} test_suite_t;

typedef struct test_run_t {
	const char * name;
	double time;
	uint32_t tests;
	uint32_t failures;
	FILE * file;
} test_run_t;

test_suite_t __test_suite;
test_case_t __test_case;
test_assertion_t __test_assertion;
test_run_t __test_run;

#ifdef STATIC_TEST_REPORT
	#define TESTS_LENGTH sizeof("tests=\"4294967295\"")
	#define FAILURES_LENGTH sizeof("failures=\"4294967295\"")
	#define TIME_LENGTH sizeof("time=\"0.000000\"")
	#define TIMESTAMP_LENGTH sizeof("timestamp=\"YYYY-MM-DDThh:mm:ssZ\"")
	#define TESTSUITE_LENGTH (sizeof("    <testsuite name=\"\">") + TESTS_LENGTH + FAILURES_LENGTH + TIME_LENGTH + TIMESTAMP_LENGTH)
	#define TESTSUITES_LENGTH (sizeof("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<testsuites name=\"\">") + TESTS_LENGTH + FAILURES_LENGTH + TIME_LENGTH)

	void test_report_suites_format() {
		const char * format = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<testsuites name=\"%s\" tests=\"%d\" failures=\"%d\" time=\"%1.6f\">";
		double clamped_time = __test_run.time > 9999999. ? 9999999. : __test_run.time;
		int maximum_length = TESTSUITES_LENGTH + strlen(__test_run.name);
		int written_length = fprintf(__test_run.file, format, __test_run.name, __test_run.tests, __test_run.failures, clamped_time);
		fprintf(__test_run.file, "%*s\n", maximum_length-written_length, "");
	}

	void test_report_open() {
		__test_run.file = fopen("report.xml", "w");
		test_report_suites_format();
	}

	void test_report_close() {
		int current_position = ftell(__test_run.file);
		fseek(__test_run.file, 0, 0);
		test_report_suites_format();
		fseek(__test_run.file, current_position, 0);
		fprintf(__test_run.file, "</testsuites>");
		fclose(__test_run.file);
	}

	void test_report_suite_format() {
		char timestamp_string[sizeof("YYYY-MM-DDThh:mm:ssZ")];
		strftime(timestamp_string, sizeof(timestamp_string), "%FT%TZ", gmtime(&__test_suite.timestamp));
		const char * format = "    <testsuite name=\"%s\" tests=\"%d\" failures=\"%d\" time=\"%1.6f\" timestamp=\"%s\">";
		double clamped_time = __test_suite.time > 9999999. ? 9999999. : __test_suite.time;
		int maximum_length = TESTSUITE_LENGTH + strlen(__test_suite.name);
		int written_length = fprintf(__test_run.file, format, __test_suite.name, __test_suite.tests, __test_suite.failures, clamped_time, timestamp_string);
		fprintf(__test_run.file, "%*s\n", maximum_length-written_length, "");
	}

	void test_report_suite_open() {
		__test_suite.start = ftell(__test_run.file);
		test_report_suite_format();
	}

	void test_report_suite_close() {
		int current_position = ftell(__test_run.file);
		fseek(__test_run.file, __test_suite.start, 0);
		test_report_suite_format();
		fseek(__test_run.file, current_position, 0);
		fprintf(__test_run.file, "    </testsuite>\n");
	}

	void test_report_case_failure() {
		fprintf(
			__test_run.file, 
			"        <testcase name=\"%s()\" time=\"%f\">\n"
			"            <failure message=\"%s\" type=\"%s\">%s:%d:    %s</failure>\n"
			"        </testcase>\n",
			__test_case.name, 
			__test_case.time,
			__test_assertion.message,
			__test_assertion.name,
			__test_assertion.file, 
			__test_assertion.line, 
			__test_assertion.expression
		);
			/*message: Die Quellcodedatei, die Zeilennummer und die Regel, gegen die verstoßen wurde.
type: Die Fehlerkategorie der Regel.*/
	}

	void test_report_case_success() {
		fprintf(
			__test_run.file, 
			"        <testcase name=\"%s()\" time=\"%f\"></testcase>\n",
			__test_case.name,
			__test_case.time
		);
	}
#else
	void test_report_open() {}
	void test_report_close() {}
	void test_report_suite_open() {}
	void test_report_suite_close() {}
	void test_report_case_failure() {}
	void test_report_case_success() {}
#endif

#ifndef STATIC_TEST_PRINT_START
	#define STATIC_TEST_PRINT_START {\
		fprintf(stderr, "\033[1m⬜ TEST %s()\033[0m\n", __test_case.name);\
	}
#endif 

#ifndef STATIC_TEST_PRINT_PASS
	#define STATIC_TEST_PRINT_PASS {\
		fprintf(stderr, "\033[1;32m✅ PASS %s() (%.3fs)\033[0m\n", __test_case.name, __test_case.time);\
	}
#endif

#ifndef STATIC_TEST_PRINT_FAIL
	#define STATIC_TEST_PRINT_FAIL {\
		fprintf(stderr, "\033[1;31m⛔ FAIL %s() (%.3fs):\033[0m %s:%d: \n    %s\n", __test_case.name, __test_case.time, __test_assertion.file, __test_assertion.line, __test_assertion.expression);\
	}
#endif 


double static_test_time() {
	struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
	return (double)time.tv_sec + 1.0e-9*time.tv_nsec;
}

time_t static_test_timestamp() {
	time_t t;
	time(&t);
	return t;
}

void static_test_suite(const char * name, void (*function)()) {
    __test_suite.name = name;
	__test_suite.timestamp = static_test_timestamp();
	__test_suite.time = static_test_time();
	__test_suite.tests = 0;
	__test_suite.failures = 0;
    test_report_suite_open();
    
	function();

	__test_suite.time = static_test_time() - __test_suite.time;
	__test_run.time += __test_suite.time;
	__test_run.tests += __test_suite.tests;
	__test_run.failures += __test_suite.failures;
    test_report_suite_close();
}

void static_test_run(const char * name, void (*function)()) {
	__test_run.name = name;
	test_report_open();
	function();
	test_report_close();
}

void static_test_case(const char * name, void (*function)()) {
    __test_case.name = name;
    __test_assertion.file = NULL;
	__test_suite.tests += 1;

    STATIC_TEST_PRINT_START

	double start = static_test_time();
	
	function();

	double end = static_test_time();
	__test_case.time = end - start;

	if(__test_assertion.file == NULL) {
        STATIC_TEST_PRINT_PASS
        test_report_case_success();
	} else {
		__test_suite.failures += 1;
        STATIC_TEST_PRINT_FAIL
		test_report_case_failure();
	}
}

void static_test_assert(const char * name, const char * file, int line, const char * expression) {
    __test_assertion.name = name;
	__test_assertion.file = file;
	__test_assertion.line = line;
	__test_assertion.expression = expression;
}

#define STATIC_TEST_ASSERT_F(assertion_name, assertion_file, assertion_line, assertion_expression, assertion_message) {\
	__test_assertion.name = assertion_name;\
	__test_assertion.file = assertion_file;\
	__test_assertion.line = assertion_line;\
	__test_assertion.expression = assertion_expression;\
	__test_assertion.message = assertion_message;\
}

// if(sizeof(__test_assertion.message) < snprintf(__test_assertion.message, sizeof(__test_assertion.message), __VA_ARGS__)) {
// 	__test_assertion.message[sizeof(__test_assertion.message) - 2] = '.';
// 	__test_assertion.message[sizeof(__test_assertion.message) - 3] = '.';
// 	__test_assertion.message[sizeof(__test_assertion.message) - 4] = '.';
// }

#define STATIC_TEST_RUN(test_run_name) static_test_run(#test_run_name, test_run_name)
#define STATIC_TEST_SUITE(test_suite_name) static_test_suite(#test_suite_name, test_suite_name)
#define STATIC_TEST_CASE(function) static_test_case(#function, function)

#define STATIC_ASSERT_(name, expression, message) if(!(expression)) { STATIC_TEST_ASSERT_F(name, __FILE__, __LINE__, #expression, message); return; }
#define STATIC_ASSERT(expression) STATIC_ASSERT_("ASSERT", expression, #expression)
#define STATIC_ASSERT_MESSAGE(expression, message) STATIC_ASSERT_("ASSERT", expression, message)

#endif /* STATIC_TEST_H */
