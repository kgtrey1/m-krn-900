#ifndef KOTP_H
#define KOTP_H

#include <linux/miscdevice.h>
#include <linux/fs.h>

typedef struct OtpMethod {
    ssize_t (*init)(void);
    ssize_t (*clean)(void);
    void (*write_fops)(struct file_operations *);
    char name[10];
    bool is_init;
    struct file_operations fops;
    struct miscdevice misc_device;
} OtpMethod_t;

typedef struct OneTimePassword {
    char *method;    
    char *password;
    bool is_assigned;
    size_t validity;
    struct OneTimePassword *next;
} OneTimePassword_t;

#define OTP_CONSUME_SUCCESS 0
#define OTP_CONSUME_NOT_FOUND 1
#define OTP_CONSUME_EXPIRED 2

void create_password(const char *method, const char *password, bool assigned, size_t validity);
size_t consume_password(const char *method, const char *password);

#endif /* KOTP_H */