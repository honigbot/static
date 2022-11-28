#include "static/test.h"
#include "static/log.h"

void pass() {
    TEST_ASSERT(1);
}

void suite0() {
}

void suite1() {
    TEST_CASE(pass);
}

void suite2() {
    TEST_CASE(pass);
    TEST_CASE(pass);
}

void run() {
    TEST_SUITE(suite0);
    TEST_SUITE(suite1);
    TEST_SUITE(suite2);
}

int main(int argc, char ** argv) {
    return TEST_RUN(run, argc, argv);
}
