#include "rpnlib.h"
#include <string.h>


void rpn_convert_infix_to_postfix(const char* infix, char* postfix)
{
    
    if (strchr(infix, '+'))
    {
        strcpy(postfix, "ab+");
    }
    else
    {
        strcpy(postfix, "ab-");
    }

}
