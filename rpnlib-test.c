#include <check.h>
#include "rpnlib.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define ck_assert_infix_conversion(INPUT,EXPECTED,ACTUAL) do { \
  const char* _ck_input = (INPUT); \
  const char* _ck_expected = (EXPECTED); \
  const char* _ck_actual = (ACTUAL); \
  ck_assert_msg(0 == strcmp(_ck_expected, _ck_actual), \
    "Failed:\nConverting \"%s\"\n Expected: \"%s\"\n Actual  : \"%s\"", _ck_input, _ck_expected, _ck_actual); \
} while (0)

struct infix_to_postfix {
    const char* infix;
    const char* postfix;
} infix_to_postfix_data[] = {
     {.infix="a+b", .postfix="ab+"}
    ,{.infix="a-b", .postfix="ab-"}
    ,{.infix="c+d", .postfix="cd+"}
    ,{.infix="a+c+d", .postfix="ac+d+"}
    ,{.infix="a+b-c", .postfix="abc-+"}
    ,{.infix="(a+b)-c", .postfix="ab+c-"}
    ,{.infix="c*d", .postfix="cd*"}
    ,{.infix="a*c+d", .postfix="ac*d+"}
    ,{.infix="a+c*d", .postfix="acd*+"}
    ,{.infix="(a+c)*d", .postfix="ac+d*"}
    ,{.infix="a+(c*d)", .postfix="acd*+"}
    ,{.infix="a+b-c*d", .postfix="abcd*-+"}
    ,{.infix="a+b*c-d", .postfix="abc*d-+"}
    ,{.infix="a+b/c-d", .postfix="abc/d-+"}
    ,{.infix="c+d*b/(a-e)", .postfix="cdbae-/*+"}
    ,{.infix="c+d*b/(a-e)^b^c", .postfix="cdbae-bc^^/*+"} //example from https://en.wikipedia.org/wiki/Shunting-yard_algorithm (result modified to meet kata precedence rules)
    ,{.infix="(a+g)*(b+a+c)^(c+e*d^f)", .postfix="ag+ba+c+cedf^*+^*"} //the example from the kata doc
};

START_TEST(when_infix_is_then_postfix_is)

    char actual_postfix[100];

    rpn_convert_infix_to_postfix(infix_to_postfix_data[_i].infix, actual_postfix);

    ck_assert_infix_conversion(infix_to_postfix_data[_i].infix, infix_to_postfix_data[_i].postfix, actual_postfix );

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
