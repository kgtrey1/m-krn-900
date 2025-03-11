#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "otpctl/subcommands.h"

static const char *VERSION = "0.1.0";

static struct option options[] = {
    {"help",    no_argument,       0, 'h'},
    {"version", no_argument,       0, 'v'},
    {0,         0,                 0,  0 }
};

static const char *options_desc[] = {
    "Show this help message.",
    "Show version.",
    0
};

static subcommand_t subcommands[] = {
    {"get",         "\tGet a value of the otp_manager module configuration.",     get},
    {"set",         "\tSet a value of the otp_manager module configuration..",    set},
    {"generate",    "Generate a new one-time password.",                        generate},
    {"validate",    "Validate a one-time password.",                            validate},
    {0,             0,                                                          0}
};

int main(int argc, char *argv[]) {
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "+hv", options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                print_usage("./otpctl [options] [subcommand]", options, options_desc, subcommands);
                return 0;
            case 'v':
                printf("otpctl version %s\n", VERSION);
                return 0;
            default:
                return 1;
        }
    }

    if (argv[optind] == NULL) {
        fprintf(stderr, "error: mandatory subcommand missing\n");
        print_usage("./otpctl [options] [subcommand]", options, options_desc, subcommands);

        return 1;
    } else {
        for (int i = 0; (size_t)i < sizeof(subcommands) / sizeof(subcommands[0]); i++) {
            if (strcmp(argv[optind], subcommands[i].name) == 0) {
                return subcommands[i].fptr(argc - optind, argv + optind);
            }
        }

        fprintf(stderr, "error: unknown subcommand: \"%s\"\n", argv[optind]);
        print_usage("./otpctl [options] [subcommand]", options, options_desc, subcommands);

        return 1;
    }
}
