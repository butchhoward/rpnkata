#include "rpnlib.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>


//todo: refactor the tests for parens, ops, and variables
//todo: check for null output buffer and buffer overrun
//todo: check for invalid variables or operators
//todo: ignore spaces
//todo: convert postfix to infix

const char LEFT_PAREN = '(';
const char RIGHT_PAREN = ')';

#define OP_STACK_SIZE 100

struct infix_params 
{
    const char* infix_in;
    char op_stack[OP_STACK_SIZE];
    int op_count;
    char* postfix_out;
};

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

static void process_left_paren(struct infix_params* params);
static void process_right_paren(struct infix_params* params);
static void process_variable(struct infix_params* params);
static void process_operator(struct infix_params *params);
static void process_operator_stack(struct infix_params *params);

void rpn_convert_infix_to_postfix(const char* infix, char* postfix)
{
    struct infix_params params;
    params.postfix_out = postfix;
    params.op_count = 0;
    memset(params.op_stack, '\0', OP_STACK_SIZE);
    *params.postfix_out = '\0';

    for (params.infix_in = infix; *params.infix_in; ++params.infix_in)
    {
        if(*params.infix_in == LEFT_PAREN)
        {
            process_left_paren(&params);
        }
        else if(*params.infix_in == RIGHT_PAREN)
        {
            process_right_paren(&params);
        }
        else if (isalpha(*params.infix_in))
        {
            process_variable(&params);
        }
        else
        {
            process_operator(&params);
        }

    }
    process_operator_stack(&params);
    
    *params.postfix_out = '\0';
}

static void process_left_paren(struct infix_params* params)
{
    params->op_stack[params->op_count++] = *params->infix_in;
    params->op_stack[params->op_count] = '\0';
}

static void process_right_paren(struct infix_params* params)
{
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

static void process_variable(struct infix_params* params)
{
    *params->postfix_out++ = *params->infix_in;
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

static void process_current_operator(op_info_t* current_op, struct infix_params* params)
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
static void process_operator(struct infix_params *params)
{
    op_info_t current_op = find_op(*params->infix_in);
    process_current_operator(&current_op, params);

    params->op_stack[params->op_count] = '\0';
}

static void process_operator_stack(struct infix_params *params)
{
    for (;params->op_count > 0;)
    {
        *params->postfix_out++ = params->op_stack[--params->op_count];
    }
    *params->postfix_out = '\0';
    params->op_stack[params->op_count] = '\0';
}
