#ifndef _SUBCOMMANDS_H_
#define _SUBCOMMANDS_H_

#include <getopt.h>

typedef struct subcommand_s {
    const char *name;
    const char *desc;
    int (*fptr)(int argc, char *argv[]);
} subcommand_t;

void print_usage(const char *exe, struct option options[], const char **option_desc, subcommand_t subcommands[]);

int get(int argc, char *argv[]);

int set(int argc, char *argv[]);

int generate(int argc, char *argv[]);

int validate(int argc, char *argv[]);

#endif