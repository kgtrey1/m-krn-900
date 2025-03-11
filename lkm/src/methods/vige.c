#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include "vige.h"

static ssize_t otp_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    pr_info("kotp: Vig read\n");
    return 0;
}

static long otp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    return 0;
}

void vige_get_fops(struct file_operations *fops)
{
    fops->owner = THIS_MODULE;
    fops->read = otp_read;
    fops->unlocked_ioctl = otp_ioctl;
    return;
}

ssize_t vige_clean()
{
    return 0;
}

ssize_t vige_init()
{
    pr_info("kotp: In vig init\n");
    return 0;
}