#define STATIC_TEST_REPORT "report.xml"
#define STATIC_TEST_PRINT stderr
#include "static/test.h"

void success() {
    STATIC_ASSERT_EQ(1, 1);
}

void fail() {
    STATIC_ASSERT_EQ(1, 2);
}

void test_suite1() {
    STATIC_TEST_CASE(success);
    STATIC_TEST_CASE(fail);
    STATIC_TEST_CASE(success);
    STATIC_TEST_CASE(fail);
}

void test_suite2() {
    STATIC_TEST_CASE(success);
    STATIC_TEST_CASE(success);
    STATIC_TEST_CASE(fail);
    STATIC_TEST_CASE(fail);
}

int main() {
    STATIC_TEST_SUITE(test_suite1);
}
