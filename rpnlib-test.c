#include <check.h>
#include "rpnlib.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


START_TEST(when_infix_is_a_plus_b_then_postfix_is_ab_plus)

    const char* infix = "a+b";
    const char* expected_postfix = "ab+";
    char actual_postfix[100];

    rpn_convert_infix_to_postfix(infix, actual_postfix);

    ck_assert_str_eq(actual_postfix, expected_postfix);

END_TEST

START_TEST(when_infix_is_a_minus_b_then_postfix_is_ab_minus)

    const char* infix = "a-b";
    const char* expected_postfix = "ab-";
    char actual_postfix[100];

    rpn_convert_infix_to_postfix(infix, actual_postfix);

    ck_assert_str_eq(actual_postfix, expected_postfix);

END_TEST

int main(void)
{
    Suite *suite = suite_create("Core");
    
    TCase *testcase = tcase_create("Core");
    suite_add_tcase(suite, testcase);

    tcase_add_test(testcase, when_infix_is_a_plus_b_then_postfix_is_ab_plus);
    tcase_add_test(testcase, when_infix_is_a_minus_b_then_postfix_is_ab_minus);


    SRunner *suite_runner = srunner_create(suite);
    srunner_run_all(suite_runner, CK_ENV);    
    int number_failed = srunner_ntests_failed(suite_runner);
    srunner_free(suite_runner);

    return number_failed == 0 ? 0 : 1;
}
