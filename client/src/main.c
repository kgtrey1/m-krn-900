#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "otp_client.h"
#include "lib_kotp.h"

int main(int argc, char *argv[])
{
    char *device_name = NULL;
    int status = 0;

    if (argc == 2 && strcmp(argv[1], "-h") == 0)
    {
        return display_help(argv[0]);
    }
    if (argc != 3)
    {
        fprintf(stderr, "Invalid usage, use -h for help.\n");
        return 1;
    }
    device_name = create_device_name(argv[1]);
    if (device_name == NULL)
    {
        return 1;
    }
    status = verify_password(device_name, argv[2]);
    free(device_name);
    return status;
}
