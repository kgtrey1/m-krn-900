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
    exec_command(argv[1], TIME_CONFIG, NULL);
    return 0;
}

static int key(char** argv, int argc) {
    if (argc != 4)
    {
        fprintf(stderr, "Invalid amount of arguments, use -h for help\n");
        return 1;
    }

    char *arg = argv[3];
    int length = strlen(arg);

    if (length > 32 || length < 3)
    {
        fprintf(stderr, "Error: Key argument must be between 3 and 32 characters\n");
        return 1;
    } 
    exec_command(argv[1], TIME_KEY, arg);
    return 0;
}

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

    exec_command(argv[1], TIME_TIME, arg);
    return 0;
}

static int help(char** argv, int argc)
{
    printf("dict commands:\n");
    printf("- config: Display the secret key and the expiration time.\n");
    printf("- key <string>: Change the secret key.\n");
    printf("- time <int>: Time in seconds before a password is expired, 0 means no expiration.\n");
    printf("- help: Display this message.\n");
    return 0;
}

int time_entry(char **argv, int argc)
{
    Command_t commands[] =
    {
        {"config", config},
        {"key", key},
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