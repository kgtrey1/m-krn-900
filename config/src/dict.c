#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include "io.h"
#include "lib_kotp.h"
#include "otp_config.h"

static int config(char** argv, int argc)
{
    if (argc != 3)
    {
        fprintf(stderr, "Invalid amount of arguments, use -h for help\n");
        return 1;
    }
    exec_command(argv[1], DICT_CONFIG, NULL);
    return 0;
}

static int add(char** argv, int argc)
{
    if (argc != 4)
    {
        fprintf(stderr, "Invalid amount of arguments, use -h for help\n");
        return 1;
    }

    char *arg = argv[3];
    int length = strlen(arg);

    if (length > 16 || length < 3)
    {
        fprintf(stderr, "Error: Add argument must be between 3 and 16 characters\n");
        return 1;
    } 
    exec_command(argv[1], DICT_ADD, arg);
    return 0;
}

static int delete(char** argv, int argc)
{
    if (argc != 4)
    {
        fprintf(stderr, "Invalid amount of arguments, use -h for help\n");
        return 1;
    }
    char *arg = argv[3];

    if (!is_integer(arg))
    {
        fprintf(stderr, "Error: Delete argument must be a valid positive number\n");
        return 1;
    }
    exec_command(argv[1], DICT_DELETE, arg);
    return 0;
}

// program name 0
// method name 1
// command name 2
// cmd arg 3

static int time(char** argv, int argc)
{
    if (argc != 4)
    {
        fprintf(stderr, "Invalid amount of arguments, use -h for help\n");
        return 1;
    }
    char *arg = argv[3];

    if (!is_integer(arg))
    {
        fprintf(stderr, "Error: time argument must be a valid positive number\n");
        return 1;
    }

    exec_command(argv[1], DICT_TIME, arg);
    return 0;
}

static int help(char** argv, int argc)
{
    printf("dict commands:\n");
    printf("- config: Display the list of word used to generate password and the expiration time.\n");
    printf("- add <string>: Add a word to the dictionary.\n");
    printf("- delete <int>: Delete a word from the dictionary, use list to get the id.\n");
    printf("- time <int>: Time in ms before a password is expired, 0 means no expiration.\n");
    printf("- help: Display this message.\n");
    return 0;
}

int dict_entry(char **argv, int argc)
{
    Command_t commands[] =
    {
        {"config", config},
        {"add", add},
        {"delete", delete},
        {"time", time},
        {"help", help},
        {NULL, NULL}
    };

    for (int i = 0; commands[i].name != NULL; i++)
    {
        if (strcmp(commands[i].name, argv[2]) == 0)
        {
            return commands[i].func(argv, argc);
        }
    }
    printf("Unknown command: %s.\nUse %s %s help for available commands.\n", argv[2], argv[0], argv[1]);
    return 1;
}