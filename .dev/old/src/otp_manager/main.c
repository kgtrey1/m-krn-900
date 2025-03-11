#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include "otp_manager/devices.h"

// Metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Valentin Cureau, Soheil Khalil, Guilhem Sante");
MODULE_DESCRIPTION("A simple OTPs (One-Time Password) manager");
MODULE_VERSION("0.1");

static dev_t otp_dev;           // Will hold the range of device numbers
static struct cdev otp_cdev0;   // cdev struct for /dev/otp0
static struct cdev otp_cdev1;   // cdev struct for /dev/otp1

// Optionally keep track of a device class if you want to use sysfs
static struct class *otp_class;

// Function called when the module is loaded
static int __init otp_manager_init(void) {
    int ret;

    // Allocate 2 device numbers (minor = 0..1) and get one major
    ret = alloc_chrdev_region(&otp_dev, 0, DEVICE_COUNT, "otp_manager");
    if (ret < 0) {
        pr_err("Failed to alloc_chrdev_region\n");
        return ret;
    }

    pr_info("Allocated device region with major=%d, minors=[%d..%d].\n",
            MAJOR(otp_dev), MINOR(otp_dev), MINOR(otp_dev) + DEVICE_COUNT - 1);

    // Initialize & add cdev for /dev/otp0
    cdev_init(&otp_cdev0, &list_based_otp_fops);
    otp_cdev0.owner = THIS_MODULE;
    ret = cdev_add(&otp_cdev0, otp_dev, 1);  // Minor = 0
    if (ret < 0) {
        pr_err("Failed to add cdev0\n");
        goto err_cdev0;
    }

    // Initialize & add cdev for /dev/otp1
    cdev_init(&otp_cdev1, &time_based_otp_fops);
    otp_cdev1.owner = THIS_MODULE;
    ret = cdev_add(&otp_cdev1, (otp_dev + 1), 1); // Minor = 1
    if (ret < 0) {
        pr_err("Failed to add cdev1\n");
        goto err_cdev1;
    }

    // Create a class so udev can create device nodes automatically
    otp_class = class_create(THIS_MODULE, "otp_class");
    if (IS_ERR(otp_class)) {
        pr_err("Failed to create class\n");
        ret = PTR_ERR(otp_class);
        goto err_class;
    }
    // Create device nodes in /dev automatically if udev is running
    device_create(otp_class, NULL, otp_dev,     NULL, "otp0");
    device_create(otp_class, NULL, otp_dev + 1, NULL, "otp1");

    pr_info("OTP Manager LKM loaded successfully.\n");
    return 0;

    // Error handling
err_class:
    cdev_del(&otp_cdev1);
err_cdev1:
    cdev_del(&otp_cdev0);
err_cdev0:
    unregister_chrdev_region(otp_dev, DEVICE_COUNT);
    return ret;
}

// Function called when the module is removed
static void __exit otp_manager_exit(void) {
    // (Optional) Destroy devices and class
    device_destroy(otp_class, otp_dev);
    device_destroy(otp_class, otp_dev + 1);
    class_destroy(otp_class);

    // Remove cdevs
    cdev_del(&otp_cdev0);
    cdev_del(&otp_cdev1);

    // Release the range of device numbers
    unregister_chrdev_region(otp_dev, DEVICE_COUNT);

    pr_info("OTP Manager LKM unloaded.\n");
}

module_init(otp_manager_init);
module_exit(otp_manager_exit);
