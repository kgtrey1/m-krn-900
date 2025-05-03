#include <stdbool.h>
#include <ctype.h>

bool is_integer(const char *str)
{
    while (*str)
    {
        if (!isdigit(*str))
        {
            return false;
        }
        str++;
    }
    return true;
}
