#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>

// todo: move that somewhere else
#define VERIFY_PASSWORD_CMD _IOW('V', 1, char *)

/**
 * Verify a password against a device using ioctl.
 */
int verify_password(const char *device, const char *password)
{
    int fd = open(device, O_RDWR);
    int result = 0;

    if (fd == -1)
    {
        perror("Error");
        fprintf(stderr, "Use the -h option to get a list of the available OTP methods.\n");
        return 1;
    }
    result = ioctl(fd, VERIFY_PASSWORD_CMD, password);

    if (result != 0)
    {
        fprintf(stderr, "Failure: Unknown or expired password\n");
        return 2;
    }
    puts("Success: Your password has been validated.");
    return 0;
}
