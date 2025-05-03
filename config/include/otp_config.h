#ifndef OTP_CLIENT_H
#define OTP_CLIENT_H

#include <stdbool.h>

typedef struct Command {
    char *name;
    int (*func)(char **argv, int argc);
} Command_t;

/* password.c */
int verify_password(const char *device, const char *password);

/* help.c */
int display_help(const char *program_name);

/* exec.c */
int exec_command(char *method, unsigned int command, void *data);

/* utils.c */
bool is_integer(const char *str);

int dict_entry(char **argv, int argc);
int time_entry(char **argv, int argc);

#endif /* OTP_CLIENT_H */