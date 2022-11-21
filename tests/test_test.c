#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct test_assertion_t {
    const char * name;
    const char * file;
    const char * expression;
    int line;
} test_assertion_t;

typedef struct test_case_t {
    const char * name;
    int result;
    void (*function)();
} test_case_t;

typedef struct test_suite_t {
    const char * name;
    size_t tests;
    FILE * file;
} test_suite_t;

test_suite_t test_suite;
test_case_t test_case;
test_assertion_t test_assertion;

void write_report(const test_suite_t * suite) {
    FILE * f = fopen("report.xml", "w");
    fprintf(f,
"<testsuite name=\"%s\">\n\
    <testcase id=\"a\" name=\"ASuccessfulTest\"/>\n\
    <testcase id=\"b\" name=\"AnotherSuccessfulTest\"/>\n\
    <testcase id=\"fail\">\n\
        <failure type=\"ASSERT_EQ\">/Users/Maximilian/Desktop/🗿 static/tests/test_test.c:57: 1 == 2</failure>\n\
    </testcase>\n\
</testsuite>"\
    , suite->name);
    fclose(f);
}

void report_test_suite_open() {
    test_suite.file = fopen("report.xml", "w");
    fprintf(test_suite.file, "<testsuite name=\"%s\">\n", test_suite.name);
}

void report_test_suite_close() {
    fprintf(test_suite.file, "</testsuite>");
    fclose(test_suite.file);
}

void static_test_suite(const char * name, void (*function)()) {
    test_suite.name = name;
    test_suite.tests = 0;
    report_test_suite_open();
    function();
    report_test_suite_close();
}

#define STATIC_TEST_SUITE(test_suite_name) static_test_suite(#test_suite_name, test_suite_name)

void static_test_failure() {
    fprintf(stderr, "\033[1;31m⛔ FAILED %s():\033[0m %s:%d: \n    %s\n", test_case.name, test_assertion.file, test_assertion.line, test_assertion.expression);
    fprintf(test_suite.file, 
    "    <testcase name=\"%s()\"/>\n"
    "        <failure type=\"%s\">%s:%d:    %s</failure>\n"
    "    </testcase>\n",
        test_case.name, test_assertion.name, test_assertion.file, test_assertion.line, test_assertion.expression);
}

void static_test_success() {
    fprintf(stderr, "\033[1;32m✅ PASSED %s()\033[0m\n", test_case.name);
    fprintf(test_suite.file, "    <testcase name=\"%s()\"/>\n", test_case.name);
}

void static_test(const char * name, void (*function)()) {
    test_case.name = name;
    test_case.result = 0;
    test_suite.tests += 1;
    fprintf(stderr, "\033[1m⬜ EXECUTING %s()\033[0m\n", test_case.name );
	function();
	if(test_assertion.file == NULL) {
        static_test_success();
	} else {
        static_test_failure();
	}
}

#define STATIC_TEST(function) static_test(#function, function)

void static_log_assert(const char * name, const char * file, int line, const char * expression) {
    test_assertion.name = name;
	test_assertion.file = file;
	test_assertion.line = line;
	test_assertion.expression = expression;
}

#define STATIC_ASSERT_(name, expression) if(!(expression)) { static_log_assert(name, __FILE__, __LINE__, #expression); return; }
#define STATIC_ASSERT(expression) STATIC_ASSERT_("ASSERT", expression)
#define STATIC_ASSERT_EQ(expected, actual) STATIC_ASSERT_("ASSERT_EQ", expected == actual)


void success() {
    STATIC_ASSERT_EQ(1, 1);
}

void fail() {
    STATIC_ASSERT_EQ(1, 2);
}

void main_suite() {
    STATIC_TEST(success);
    STATIC_TEST(fail);
    STATIC_TEST(success);
    STATIC_TEST(success);
    STATIC_TEST(success);
    STATIC_TEST(fail);
    STATIC_TEST(fail);
    STATIC_TEST(fail);
}

int main() {
    STATIC_TEST_SUITE(main_suite);
    // STATIC_TEST_SUITE(main_suite);
}
