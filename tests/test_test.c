#define STATIC_TEST_REPORT "report.xml"
#define STATIC_TEST_PRINT stderr

#define TEST_REPORT_PRINT stderr
#define TEST_REPORT_FILE "report.xml"

#include "static/test.h"

void success() {
    STATIC_ASSERT(1 == 1);
}

void fail() {
    STATIC_ASSERT_MESSAGE(1 == 2, "1 should be equal 2");
}


void test_suite_1() {
    STATIC_TEST_CASE(success);
    STATIC_TEST_CASE(fail);
}

void test_suite_2() {
    STATIC_TEST_CASE(fail);
    STATIC_TEST_CASE(success);
    STATIC_TEST_CASE(fail);
    STATIC_TEST_CASE(fail);
    STATIC_TEST_CASE(success);
    STATIC_TEST_CASE(success);
}

void test_run() {
    STATIC_TEST_SUITE(test_suite_1);
    STATIC_TEST_SUITE(test_suite_2);
}

int main() {
    STATIC_TEST_RUN(test_run);
}
