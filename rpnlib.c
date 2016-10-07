#include "rpnlib.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


//todo: check for null output buffer and buffer overrun
//todo: convert postfix to infix

const char LEFT_PAREN = '(';
const char RIGHT_PAREN = ')';

typedef struct infix_result_t 
{
    char* op_stack;
    int op_count;
    char* postfix_out;
} infix_result_t;

enum associativity_t {NONE, LEFT, RIGHT};

typedef struct op_info {
    char op;
    int precedence;
    enum associativity_t associativity;
} op_info_t;

const op_info_t op_data_nil = { .op='\0', .precedence = 0, .associativity=NONE};

const op_info_t op_data[] = {
     {.op='+', .precedence=1, .associativity=LEFT}
    ,{.op='-', .precedence=2, .associativity=LEFT}
    ,{.op='*', .precedence=3, .associativity=LEFT}
    ,{.op='/', .precedence=4, .associativity=LEFT}
    ,{.op='^', .precedence=5, .associativity=RIGHT}
    ,{.op='\0', .precedence=0, .associativity=NONE}
};

static bool check_for_left_paren(char c);
static void process_left_paren(char paren, infix_result_t* params);

static bool check_for_right_paren(char c);
static void process_right_paren(char paren, infix_result_t* params);

static bool check_for_variable(char c);
static void process_variable(char var, infix_result_t* params);

static bool check_for_operator(char c);
static void process_operator(char op, infix_result_t *params);

static bool check_for_space(char c);
static void process_space(char c, infix_result_t *params);

static bool process_a_character(char c, infix_result_t* params);

typedef bool (*checkfn_t)(char c);
typedef void (*processfn_t)(char c, infix_result_t* p);

typedef struct dispatch_info_t {
    checkfn_t check;
    processfn_t process;
} dispatch_info_t;

dispatch_info_t dispatch[] = {
    {.check=check_for_left_paren, .process=process_left_paren}
    ,{.check=check_for_right_paren, .process=process_right_paren}
    ,{.check=check_for_variable, .process=process_variable}
    ,{.check=check_for_operator, .process=process_operator}
    ,{.check=check_for_space, .process=process_space}
    ,{.check=NULL, .process=NULL}
};

static void process_operator_stack(infix_result_t *params);

void rpn_convert_infix_to_postfix(const char* infix, char* postfix)
{
    if(NULL == postfix)
    {
        return;
    }

    size_t op_stack_buffer_len = strlen(infix+1);
    char op_stack_buffer[op_stack_buffer_len];

    infix_result_t params;
    params.postfix_out = postfix;
    params.op_count = 0;
    params.op_stack = &op_stack_buffer[0];
    memset(params.op_stack, '\0', op_stack_buffer_len);
    *params.postfix_out = '\0';

    for (const char *infix_in = infix; *infix_in; ++infix_in)
    {
        if (!process_a_character(*infix_in, &params))
        {
            *postfix = '\0';
            return;
        }
    }
    process_operator_stack(&params);
    
    *params.postfix_out = '\0';
}

static bool process_a_character(char c, infix_result_t* params)
{
    bool dispatched = false;
    for(int i = 0;dispatch[i].check != NULL;++i)
    {
        if(dispatch[i].check(c))
        {
            dispatched = true;
            dispatch[i].process(c, params);
        }
    }   
    return dispatched;
}

static bool check_for_space(char c)
{
    return isspace(c);
}

static void process_space(char c, infix_result_t *params)
{
    (void)c;
    (void)params;
    return;
}

static bool check_for_left_paren(char c)
{
    return LEFT_PAREN == c;
}

static void process_left_paren(char paren, infix_result_t* params)
{
    params->op_stack[params->op_count++] = paren;
    params->op_stack[params->op_count] = '\0';
}

static bool check_for_right_paren(char c)
{
    return RIGHT_PAREN == c;
}

static void process_right_paren(char paren, infix_result_t* params)
{
    (void)paren;

    for (; params->op_count > 0;)
    {
        char op = params->op_stack[--params->op_count];
        if (params->op_stack[params->op_count] != LEFT_PAREN)
        {
            *params->postfix_out++ = op;
            *params->postfix_out = '\0';
        }
        else
        {
            break;
        }
    }
    params->op_stack[params->op_count] = '\0';
}

static bool check_for_variable(char c)
{
    return islower(c);
}

static void process_variable(char var, infix_result_t* params)
{
    *params->postfix_out++ = var;
    *params->postfix_out = '\0';
}

static op_info_t find_op(char op)
{
    for (int i = 0; op_data[i].op != '\0'; ++i)
    {
        if (op_data[i].op == op)
        {
            return op_data[i];
        }
    }

    return op_data_nil;
}

static bool check_for_operator(char c)
{
    op_info_t op_info = find_op(c);
    return op_info.op != op_data_nil.op;
}

static bool top_op_on_stack_has_correct_precendence_then_pop_it_to_the_output(op_info_t* current_op, infix_result_t* params)
{
    bool has_op_that_meets_precendence_rules = false;

    op_info_t top_op = find_op( params->op_stack[params->op_count - 1] );
        
    if ( (current_op->associativity == LEFT && current_op->precedence <= top_op.precedence) || 
         (current_op->associativity == RIGHT && current_op->precedence < top_op.precedence)
    )
    {
        *params->postfix_out++ = top_op.op;
        *params->postfix_out = '\0';

        --params->op_count;
        params->op_stack[params->op_count] = '\0';

        has_op_that_meets_precendence_rules = true;
    }

    return has_op_that_meets_precendence_rules;
}

static void process_current_operator(op_info_t* current_op, infix_result_t* params)
{
    for (;params->op_count > 0;)
    {
        if(!top_op_on_stack_has_correct_precendence_then_pop_it_to_the_output(current_op,params))
        {
            break;
        }
    }
    params->op_stack[params->op_count++] = current_op->op;
}
static void process_operator(char op, infix_result_t *params)
{
    op_info_t current_op = find_op(op);
    process_current_operator(&current_op, params);

    params->op_stack[params->op_count] = '\0';
}

static void process_operator_stack(infix_result_t *params)
{
    for (;params->op_count > 0;)
    {
        *params->postfix_out++ = params->op_stack[--params->op_count];
    }
    *params->postfix_out = '\0';
    params->op_stack[params->op_count] = '\0';
}
