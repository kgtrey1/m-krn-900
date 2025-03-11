#include <linux/cdev.h>
#include <linux/fs.h>

#include "otp_manager/ioctl.h"

static long handle_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;

    switch (cmd) {
        default:
        pr_info("otp_ioctl: unknown cmd %u\n", cmd);
        ret = -ENOTTY;
        break;
    }

    return ret;
}

struct file_operations time_based_otp_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = handle_ioctl,
};
