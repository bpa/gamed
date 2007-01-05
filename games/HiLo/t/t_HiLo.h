#include <cunit/testcase.h>
#include <stdio.h>

/* TEST_CASE(HiLo); */
#define TEST_NAME HiLo

SETUP {
   printf("In setup\n");
}

TEST(fail) {
    assert(1 == 2);
}

TEST(pass) {
    assert_equal(1, 2);
}
