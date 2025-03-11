#include <stdlib.h>
#include <stdio.h>
#include "lib_kotp.h"

int display_help(const char *program_name)
{
    otp_method_list_t *methods = NULL;
    int status = 0;

    methods = get_available_otp_methods(&status);
    printf("Usage: %s <OTP method> <password>\n", program_name);
    printf("Available methods:\n");
    if (status != 0)
    {
        return 1;
    }
    if (methods == NULL)
    {
        printf("None.\n");
    }
    else
    {
        for (otp_method_list_t *tmp = methods; tmp != NULL; tmp = tmp->next)
        {
            printf("- %s\n", tmp->name);
        }
        free_otp_methods_list(methods);
    }
    return 0;
}