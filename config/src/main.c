#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "otp_config.h"
#include "lib_kotp.h"

int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "-h") == 0)
    {
        return display_help(argv[0]);
    }
    if (argc < 3)
    {
        fprintf(stderr, "Invalid usage, use -h for help.\n");
        return 1;
    }
    if (strcmp(argv[1], "dict") == 0)
    {
        return dict_entry(argv, argc);
    }
    if (strcmp(argv[1], "time") == 0)
    {
        return time_entry(argv, argc);
    }
    fprintf(stderr, "Error: unknown method: %s. Use -h for help.\n", argv[1]);
    return 1;
}
