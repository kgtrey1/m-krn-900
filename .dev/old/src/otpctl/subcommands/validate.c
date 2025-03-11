#include <stdio.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "otpctl/subcommands.h"
#include "otpctl/otp_manager.h"

static struct option options[] = {
    {"help",        no_argument,        0, 'h'},
    {"password",    required_argument,  0, 'p'},
    {0,             0,                  0,  0 }
};

static const char *options_desc[] = {
    "Show this help message.",
    "The password to validate. (can't be longer than 32 character)",
    0
};

static subcommand_t subcommands[] = {
    {0, 0,  0}
};

int validate(int argc, char *argv[]) {
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "hp:", options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                print_usage("./otpctl validate [options]", options, options_desc, subcommands);
                return 0;
            case 'p':
                int fd;
                struct otp_verify_args otp_verify;

                if (strlen(optarg) > 32) {
                    fprintf(stderr, "error: given password can't be longer than 32 characters.\n");

                    return 1;
                }
                strcpy(otp_verify.otp, optarg);

                fd = open(LIST_BASED_OTP_MANAGER_DEVICE, O_RDWR);
                if (fd < 0) {
                    perror("open");
                    close(fd);

                    return 1;
                }

                if (ioctl(fd, OTP_IOCTL_VERIFY, &otp_verify) < 0) {
                    perror("ioctl set key");
                    close(fd);

                    return 1;
                }

                if (otp_verify.valid) {
                    printf("password valid.\n");
                } else {
                    printf("password invalid.\n");
                }

                close(fd);

                return 0;
            default:
                return 1;
        }
    }

    return 1;
}
