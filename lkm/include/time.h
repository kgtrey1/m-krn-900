#ifndef OTP_TIME_H
#define OTP_TIME_H

#include <linux/fs.h>

#define MAX_KEY_LENGTH 33
#define MAX_TIME_PASSWORD_LENGTH 16

void totp_get_fops(struct file_operations *fops);
ssize_t totp_clean(void);
ssize_t totp_init(void);

#endif /* OTP_TIME_H */