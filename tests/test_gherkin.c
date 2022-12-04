#define TEST_REPORT "report_bdd.xml"
#include "static/test.h"

void compare_numbers() {
    // setup
    int a = 0;
    int b = 0;

    TEST_WHEN("a==1 and b==2") {
        a += 1;
        b += 2;

        TEST_EXPECT(a == b);
    }

    TEST_WHEN("a==1 and b==2") {
        a += 1;
        b += 2;
        
        TEST_THEN("a should be unequal b") {
            TEST_EXPECT(a == b);
        }
    }

    // shutdown
}

void suite() {
    TEST_SCENARIO(compare_numbers);
}

void run() {
    TEST_SUITE(suite);
}

int main(int argc, char ** argv) {
    return TEST_RUN(argc, argv, run);
}
