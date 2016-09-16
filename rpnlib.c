#include "rpnlib.h"
#include <string.h>
#include <ctype.h>




void rpn_convert_infix_to_postfix(const char* infix, char* postfix)
{
    
    char* postfix_out = postfix;
    char op_stack[100];
    int op_count = 0;    
    for (const char* infix_in = infix; *infix_in; ++infix_in)
    {
        if(*infix_in == '(')
        {
            op_stack[op_count++] = *infix_in;
        }
        else if(*infix_in == ')')
        {
            for (;op_count > 0;)
            {
                char op = op_stack[--op_count];
                if (op_stack[op_count] != '(')
                {
                    *postfix_out++ = op;
                }
                else
                {
                    break;
                }
            }
        }
        else if (isalpha(*infix_in))
        {
            *postfix_out++ = *infix_in;
         }
        else
        {
            char current_op = *infix_in;
            if (op_count > 0 )
            {
                char top_op = op_stack[op_count - 1];
                if (top_op == '*' && current_op == '+' )
                {
                    *postfix_out++ = op_stack[--op_count];
                }
            }
            op_stack[op_count++] = current_op;
        }
    }
    for (;op_count > 0;)
    {
        *postfix_out++ = op_stack[--op_count];
    }
    *postfix_out = '\0';
}
