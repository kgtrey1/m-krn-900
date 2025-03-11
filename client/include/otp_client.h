#ifndef OTP_CLIENT_H
#define OTP_CLIENT_H

/* password.c */
int verify_password(const char *device, const char *password);

/* help.c */
int display_help(const char *program_name);

#endif /* OTP_CLIENT_H */