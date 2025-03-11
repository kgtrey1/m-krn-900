#include <stdlib.h>
#include <stdio.h>

#include "otpctl/subcommands.h"

void print_usage(const char *exe, struct option options[], const char **option_desc, subcommand_t subcommands[]) {
    printf("Usage: %s\n", exe);

    if (options[0].name != 0) {
        printf("Options:\n");
        for (int i = 0; options[i].name != 0; i++) {
            printf("\t-%c, --%s\t\t%s\n", (char)options[i].val, options[i].name, option_desc[i]);
        }
    }

    if (subcommands[0].name != 0) {
        printf("Subcommands:\n");
        for (int i = 0; subcommands[i].name != 0; i++) {
            printf("\t%s\t\t%s\n", subcommands[i].name, subcommands[i].desc);
        }
    }
}
