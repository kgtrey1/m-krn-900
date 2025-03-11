#ifndef _DEVICES_H_
#define _DEVICES_H_

#include <linux/fs.h>

#define DEVICE_COUNT 2

extern struct file_operations time_based_otp_fops;

extern struct file_operations list_based_otp_fops;

#endif