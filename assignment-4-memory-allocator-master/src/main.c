#include "tests.h"

#define TESTS_NUM 5

int main() {
    int tests_passed = 0;
    if (test1()) {
        tests_passed++;
    };
    if (test2()) {
        tests_passed++;
    };
    if (test3()) {
        tests_passed++;
    };
    if (test4()) {
        tests_passed++;
    };
    if (test5()) {
        tests_passed++;
    };

    if (tests_passed == TESTS_NUM) {
        printf("All tests passed");
        return 0;
    }
    printf("Some of the tests failed");
}
