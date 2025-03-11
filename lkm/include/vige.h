#ifndef OTP_VIGE_H
#define OTP_VIGE_H

#include <linux/uaccess.h>
#include <linux/fs.h>

void vige_get_fops(struct file_operations *fops);
ssize_t vige_clean(void);
ssize_t vige_init(void);

#endif /* OTP_VIGE_H */