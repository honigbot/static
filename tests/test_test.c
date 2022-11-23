#include "static/test.h"

void fail() {
    TEST_ASSERT_MESSAGE(1 <= 0, "error: %d is bigger than %d", 1, 0);
}

void pass() {
    TEST_ASSERT(1 >= 0);
}

void suite1() {
    TEST_CASE(fail);
    TEST_CASE(pass);
}

void suite2() {
    TEST_CASE(pass);
    TEST_CASE(fail);
}

void run() {
    TEST_SUITE(suite1);
    TEST_SUITE(suite2);
}

int main() {
    return TEST_RUN(run);
}
