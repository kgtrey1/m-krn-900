#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include "io.h"

/**
 * Verify a password against a device using ioctl.
 */
int verify_password(const char *device, const char *password)
{
    int fd = open(device, O_RDWR);
    int result = 0;
    data_transfer_t req = {
        .data = (void *)password,
        .size = sizeof(char) * (strlen(password) + 1)
    };

    if (fd == -1)
    {
        perror("Error");
        fprintf(stderr, "Use the -h option to get a list of the available OTP methods.\n");
        return 1;
    }
    result = ioctl(fd, VERIFY_PASSWORD_CMD, &req);

    if (result != 0)
    {
        fprintf(stderr, "%s", req.resp);
        return 2;
    }
    printf("%s", req.resp);
    return 0;
}
