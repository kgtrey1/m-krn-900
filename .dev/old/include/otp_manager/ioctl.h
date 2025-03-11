#ifndef _IOCTL_H_
#define _IOCTL_H_

#include <linux/ioctl.h>

#define OTP_MAGIC 'o'

#define OTP_IOCTL_SET_KEY   _IOW(OTP_MAGIC, 1, struct otp_key)
#define OTP_IOCTL_GET_KEY   _IOR(OTP_MAGIC, 2, struct otp_key)
#define OTP_IOCTL_ADD_PASS  _IOW(OTP_MAGIC, 3, struct otp_pass)
#define OTP_IOCTL_VERIFY    _IOWR(OTP_MAGIC, 4, struct otp_verify_args)


struct otp_key {
    char key[64];  // store a secret key
};

struct otp_pass {
    char pass[32]; // store a password for list-based method
};

struct otp_verify_args {
    char otp[32];  // the OTP to check
    int  valid;   // store verification result (1 = valid, 0 = invalid)
};

#endif