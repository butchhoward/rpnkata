#include "rpnlib.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


//todo: refactor the tests for parens, ops, and variables
//todo: check for null output buffer and buffer overrun
//todo: check for invalid variables or operators
//todo: ignore spaces
//todo: convert postfix to infix

const char LEFT_PAREN = '(';
const char RIGHT_PAREN = ')';

#define OP_STACK_SIZE 100

typedef struct infix_result_t 
{
    char op_stack[OP_STACK_SIZE];
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
};

const int op_data_count = sizeof op_data / sizeof op_data[0];

static bool check_for_left_paren(char var);
static void process_left_paren(char var, infix_result_t* params);

static bool check_for_right_paren(char var);
static void process_right_paren(char var, infix_result_t* params);

static bool check_for_variable(char var);
static void process_variable(char var, infix_result_t* params);

static bool check_for_operator(char var);
static void process_operator(char op, infix_result_t *params);

static void process_a_character(char infix_in, infix_result_t* params);

typedef bool (*checkfn_t)(char c);
typedef void (*processfn_t)(char c, infix_result_t* p);

typedef struct dispatch_info_t {
    checkfn_t check;
    processfn_t process;
} dispatch_info_t;

dispatch_info_t dispatch[] = {
    {.check=check_for_left_paren, .process=process_left_paren},
    {.check=check_for_right_paren, .process=process_right_paren},
    {.check=check_for_variable, .process=process_variable},
    {.check=check_for_operator, .process=process_operator},
    {.check=NULL, .process=NULL}
};

static void process_operator_stack(infix_result_t *params);

void rpn_convert_infix_to_postfix(const char* infix, char* postfix)
{
    infix_result_t params;
    params.postfix_out = postfix;
    params.op_count = 0;
    memset(params.op_stack, '\0', OP_STACK_SIZE);
    *params.postfix_out = '\0';

    for (const char *infix_in = infix; *infix_in; ++infix_in)
    {
        process_a_character(*infix_in, &params);
    }
    process_operator_stack(&params);
    
    *params.postfix_out = '\0';
}

static void process_a_character(char infix_in, infix_result_t* params)
{
    if(check_for_left_paren(infix_in))
    {
        process_left_paren(infix_in, params);
    }
    else if(check_for_right_paren(infix_in))
    {
        process_right_paren(infix_in, params);
    }
    else if (check_for_variable(infix_in))
    {
        process_variable(infix_in, params);
    }
    else if (check_for_operator(infix_in))
    {
        process_operator(infix_in, params);
    }
    else
    {
        //ignore the character
    }
}

static bool check_for_left_paren(char var)
{
    return LEFT_PAREN == var;
}

static void process_left_paren(char var, infix_result_t* params)
{
    params->op_stack[params->op_count++] = var;
    params->op_stack[params->op_count] = '\0';
}

static bool check_for_right_paren(char var)
{
    return RIGHT_PAREN == var;
}

static void process_right_paren(char var, infix_result_t* params)
{
    (void)var;

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

static bool check_for_variable(char var)
{
    return isalpha(var);
}

static void process_variable(char var, infix_result_t* params)
{
    *params->postfix_out++ = var;
    *params->postfix_out = '\0';
}

static op_info_t find_op(char op)
{
    for (int i = 0; i < op_data_count; ++i)
    {
        if (op_data[i].op == op)
        {
            return op_data[i];
        }
    }

    return op_data_nil;
}

static bool check_for_operator(char op)
{
    op_info_t op_info = find_op(op);
    return op_info.op != op_data_nil.op;
}

static void process_current_operator(op_info_t* current_op, infix_result_t* params)
{
    for (;params->op_count > 0;)
    {
        op_info_t top_op = find_op( params->op_stack[params->op_count - 1] );
        
        if ( (current_op->associativity == LEFT && current_op->precedence <= top_op.precedence) || 
             (current_op->associativity == RIGHT && current_op->precedence < top_op.precedence)
        )
        {
            *params->postfix_out++ = top_op.op;
            *params->postfix_out = '\0';

            --params->op_count;
            params->op_stack[params->op_count] = '\0';
        }
        else
        {
            break;
        }
    }
    params->op_stack[params->op_count++] = current_op->op;
}
static void process_operator(char infix_in, infix_result_t *params)
{
    op_info_t current_op = find_op(infix_in);
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
