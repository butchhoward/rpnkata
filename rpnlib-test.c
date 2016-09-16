#include <check.h>
#include "rpnlib.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct infix_to_postfix {
    const char* infix;
    const char* postfix;
} infix_to_postfix_data[] = {
     {.infix="a+b", .postfix="ab+"}
    ,{.infix="a-b", .postfix="ab-"}
    ,{.infix="c+d", .postfix="cd+"}
    ,{.infix="a+c+d", .postfix="ac+d+"}
    ,{.infix="c*d", .postfix="cd*"}
};

START_TEST(when_infix_is_then_postfix_is)

    char actual_postfix[100];

    rpn_convert_infix_to_postfix(infix_to_postfix_data[_i].infix, actual_postfix);

    ck_assert_str_eq(actual_postfix, infix_to_postfix_data[_i].postfix);

END_TEST


int main(void)
{
    Suite *suite = suite_create("Core");
    
    TCase *testcase = tcase_create("Core");
    suite_add_tcase(suite, testcase);

    tcase_add_loop_test(testcase, when_infix_is_then_postfix_is, 0, sizeof infix_to_postfix_data / sizeof infix_to_postfix_data[0]);


    SRunner *suite_runner = srunner_create(suite);
    srunner_run_all(suite_runner, CK_ENV);    
    int number_failed = srunner_ntests_failed(suite_runner);
    srunner_free(suite_runner);

    return number_failed == 0 ? 0 : 1;
}
