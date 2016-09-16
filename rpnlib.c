#include "rpnlib.h"
#include <string.h>
#include <ctype.h>


void rpn_convert_infix_to_postfix(const char* infix, char* postfix)
{
    
    char* postfix_out = postfix;
    char op = '\0';    
    for (const char* infix_in = infix; *infix_in; ++infix_in)
    {
        if (isalpha(*infix_in))
        {
            *postfix_out++ = *infix_in;
            if(op)
            {
                *postfix_out++ = op;
                op = '\0';
            }
        }
        else
        {
            op = *infix_in;
        }
    }
    *postfix_out = '\0';
}
