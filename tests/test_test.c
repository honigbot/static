#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

FILE * report = NULL;
const char * current_file = NULL;
int current_line = 0;
const char * current_expression = NULL;

#ifndef STATIC_TEST_EXECUTE_MESSAGE
    #define STATIC_TEST_EXECUTE_MESSAGE(name) {\
        fprintf(stderr, "\033[1m⬜ EXECUTING %s()\033[0m\n", current_name);\
    }
#endif

#ifndef STATIC_TEST_SUCCESS_MESSAGE
    #define STATIC_TEST_SUCCESS_MESSAGE(name) {\
        fprintf(stderr, "\033[1;32m✅ PASSED %s()\033[0m\n", name);\
    }
#endif

#ifndef STATIC_TEST_FAILURE_MESSAGE
    #define STATIC_TEST_FAILURE_MESSAGE(name) {\
        fprintf(stderr, "\033[1;31m⛔ FAILED %s():\033[0m %s:%d: \n    %s\n", name, current_file, current_line, current_expression);\
    }
#endif

int STATIC_execute(const char* current_name, void (*current_function)()) {
    STATIC_TEST_EXECUTE_MESSAGE(current_name);
	current_function();
	if(current_file == NULL) {
        STATIC_TEST_SUCCESS_MESSAGE(current_name);
		return 0;
	} else {
        STATIC_TEST_FAILURE_MESSAGE(current_name);
		return -1;
	}
}

int STATIC_ASSERT_(const char * file, int line, const char * expression, int pass) {
	if(pass) return 1;
	current_file = file;
	current_line = line;
	current_expression = expression;
	return 0;
}

// #define STATIC_TEST(function) if(STATIC_execute((#function), function) == -1) return -1;

#define STATIC_ASSERT(expression) if (!STATIC_ASSERT_(__FILE__, __LINE__, (#expression), (expression) ? 1 : 0)) return
#define STATIC_ASSERT_EQ(expected, actual) STATIC_ASSERT(expected == actual)
#define STATIC_ASSERT_NE(expected, actual) STATIC_ASSERT(expected != actual)
#define STATIC_ASSERT_GE(expected, actual) STATIC_ASSERT(expected >= actual)
#define STATIC_ASSERT_LE(expected, actual) STATIC_ASSERT(expected <= actual)
#define STATIC_ASSERT_GT(expected, actual) STATIC_ASSERT(expected > actual)
#define STATIC_ASSERT_LT(expected, actual) STATIC_ASSERT(expected < actual)
#define STATIC_ASSERT_EXIT

#include <stdlib.h>

typedef struct test_t {
    const char * name;
    int result;
    void (*function)();
    struct test_t * next_test;
} test_t;

typedef struct test_suite_t {
    const char * name;
    size_t tests;
    test_t * first_test;
    FILE * file;
} test_suite_t;

test_suite_t test_suite;

void write_report(const test_suite_t * suite) {
    FILE * f = fopen("report.xml", "w");
    fprintf(f,
"<testsuite name=\"%s\" tests=\"%zu\">\n\
    <testcase classname=\"a\" name=\"ASuccessfulTest\"/>\n\
    <testcase classname=\"b\" name=\"AnotherSuccessfulTest\"/>\n\
    <testcase name=\"fail\">\n\
        <failure type=\"ASSERT_EQ\">/Users/Maximilian/Desktop/🗿 static/tests/test_test.c:57: 1 == 2</failure>\n\
    </testcase>\n\
</testsuite>"\
    , suite->name, suite->tests);
    fclose(f);
}

#define STATIC_TEST_SUITE(test_suite_name) {\
    test_suite.name = #test_suite_name;\
    test_suite.tests = 0;\
    test_suite_name();\
    write_report(&test_suite);\
}

#define STATIC_TEST(function) {\
    test_t test; \
    test.name = #function; \
    test.result = 0;\
    test_suite.tests += 1;\
}

/*
<testsuite name="main_suite" tests="3">
    <testcase classname="foo1" name="ASuccessfulTest"/>
    <testcase classname="foo2" name="AnotherSuccessfulTest"/>
    <testcase name="fail">
        <failure type="ASSERT_EQ">/Users/Maximilian/Desktop/🗿 static/tests/test_test.c:57: 1 == 2</failure>
    </testcase>
</testsuite>
*/

void success() {
    STATIC_ASSERT_EQ(1, 1);
}

void fail() {
    STATIC_ASSERT_EQ(1, 2);
}

void main_suite() {
    STATIC_TEST(success);
    STATIC_TEST(fail);
}

int main() {
    STATIC_TEST_SUITE(main_suite);
    // STATIC_TEST_SUITE(main_suite);
}
