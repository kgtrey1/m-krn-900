#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "lib_kotp.h"
#include "io.h"


int exec_command(char *method, unsigned int command, void *data)
{
    char *device = create_device_name(method);
    if (device == NULL)
    {
        return 1;
    }
    int fd = open(device, O_RDWR);
    int result = 0;
    data_transfer_t req = {
        .data = data,
        .size = sizeof(data)
    };

    if (fd == -1)
    {
        perror("Error");
        fprintf(stderr, "Use the -h option to get a list of the available OTP methods.\n");
        return 1;
    }
    result = ioctl(fd, command, &req);

    if (result != 0)
    {
        fprintf(stderr, "%s", req.resp);
        return 2;
    }
    printf("%s", req.resp);
    return 0;
}