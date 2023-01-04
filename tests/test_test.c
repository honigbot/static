#include "static/test.h"

void pass1() {
    test_assert(1);
}

void pass2() {
    const char * a = "abcd";
    const char * b = "abcd";
    test_assert_eq_string(a, b);
} 

void fail1() {
    int a = 0;
    int b = 1;
    test_assert_eq_int(a, b);
}

void fail2() {
    int a = 1;
    test_expect(a);
    test_expect(1);
    int b = 0;
    test_expect(b);
    test_expect(0);
}

void suite1() {
    test_case(pass1);
}

void suite2() {
    test_case(pass1);
    test_case(pass2);
}

int main() {
    suite1();
    suite2();
}
