#include <stdio.h>

#include "otpctl/subcommands.h"

static struct option options[] = {
    {"help",    no_argument,       0, 'h'},
    {0,         0,                 0,  0 }
};

static const char *options_desc[] = {
    "Show this help message.",
    0
};

static subcommand_t subcommands[] = {
    {0, 0,  0}
};

int generate(int argc, char *argv[]) {
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "h", options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                print_usage("./otpctl generate [options]", options, options_desc, subcommands);
                return 0;
            default:
                return 1;
        }
    }

    printf("\"generate\" subcommand invoked!\n");

    return 0;
}
