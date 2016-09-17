#include "rpnlib.h"
#include <string.h>
#include <ctype.h>

const char LEFT_PAREN = '(';
const char RIGHT_PAREN = ')';
const char OP_ADD = '+';
const char OP_MULT = '*';
const char OP_MINUS = '-';

struct infix_params 
{
    const char* infix_in;
    char op_stack[100];
    int op_count;
    char* postfix_out;
};

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

static void process_operator(struct infix_params *params)
{
    char current_op = *params->infix_in;
    if (params->op_count > 0)
    {
        char top_op = params->op_stack[params->op_count - 1];
        if (top_op == OP_MULT && (current_op == OP_ADD || current_op == OP_MINUS))
        {
            *params->postfix_out++ = params->op_stack[--params->op_count];
        }
    }
    params->op_stack[params->op_count++] = current_op;
}

static void process_operator_stack(struct infix_params *params)
{
    for (;params->op_count > 0;)
    {
        *params->postfix_out++ = params->op_stack[--params->op_count];
    }
    
}
