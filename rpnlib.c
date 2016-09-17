#include "rpnlib.h"
#include <string.h>
#include <ctype.h>

const char LEFT_PAREN = '(';
const char RIGHT_PAREN = ')';
const char OP_ADD = '+';
const char OP_MULT = '*';
const char OP_MINUS = '-';
const char OP_DIV = '/';

struct infix_params 
{
    const char* infix_in;
    char op_stack[100];
    int op_count;
    char* postfix_out;
};

typedef struct op_info {
    char op;
    int precedence;
} op_info_t;

const op_info_t op_data_nil = { .op='\0', .precedence = 0};

const op_info_t op_data[] = {
     {.op='+', .precedence=2}
    ,{.op='-', .precedence=2}
    ,{.op='*', .precedence=3}
    ,{.op='/', .precedence=3}
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
}

static void process_right_paren(struct infix_params* params)
{
    for (; params->op_count > 0;)
    {
        char op = params->op_stack[--params->op_count];
        if (params->op_stack[params->op_count] != LEFT_PAREN)
        {
            *params->postfix_out++ = op;
        }
        else
        {
            break;
        }
    }
}

static void process_variable(struct infix_params* params)
{
    *params->postfix_out++ = *params->infix_in;
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

static void process_operator(struct infix_params *params)
{
    op_info_t current_op = find_op(*params->infix_in);
    for (;params->op_count > 0;)
    {
        op_info_t top_op = find_op( params->op_stack[params->op_count - 1] );
        
        if ( current_op.precedence < top_op.precedence )
        {
            *params->postfix_out++ = top_op.op;
            --params->op_count;
        }
        else
        {
            break;
        }
    }
    params->op_stack[params->op_count++] = current_op.op;
}

static void process_operator_stack(struct infix_params *params)
{
    for (;params->op_count > 0;)
    {
        *params->postfix_out++ = params->op_stack[--params->op_count];
    }
    
}
