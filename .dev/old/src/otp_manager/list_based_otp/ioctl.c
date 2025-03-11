#include <linux/cdev.h>
#include <linux/fs.h>

#include "otp_manager/ioctl.h"

static long handle_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;
    struct otp_verify_args verify_otp;

    switch (cmd) {
        case OTP_IOCTL_VERIFY:
        if (copy_from_user(&verify_otp, (void __user *)arg, sizeof(struct otp_verify_args))) {
            return -EFAULT;
        }

        // TODO: make a proper function to verify the password

        pr_info("received key: %s\n", verify_otp.otp);
        verify_otp.valid = 1;

        if (copy_to_user((void __user *)arg, &verify_otp, sizeof(struct otp_verify_args))) {
            return -EFAULT;
        }
        break;
        
        default:
        pr_info("otp_ioctl: unknown cmd %u\n", cmd);
        ret = -ENOTTY;
        break;
    }

    return ret;
}

struct file_operations list_based_otp_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = handle_ioctl,
};