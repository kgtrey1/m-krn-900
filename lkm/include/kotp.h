#ifndef KOTP_H
#define KOTP_H

#include <linux/miscdevice.h>

typedef struct OtpMethod {
    ssize_t (*init)(void);
    ssize_t (*clean)(void);
    void (*write_fops)(struct file_operations *);
    char name[10];
    bool is_init;
    struct file_operations fops;
    struct miscdevice misc_device;
} OtpMethod_t;

#endif /* KOTP_H */