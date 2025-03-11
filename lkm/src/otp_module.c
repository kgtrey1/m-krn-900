#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/slab.h> 
#include <linux/string.h> 

#include "kotp.h"
#include "vige.h"

OtpMethod_t methods[] = {
    { vige_init, vige_clean, vige_get_fops, "vige", false },
};
const int otp_amount = 1;

static const char *create_device_name(const char *method_name)
{
    char *name = NULL;
    const int allocation_size =
        sizeof(char) * (strlen("kotp_") + strlen(method_name) + 1);

    name = kmalloc(allocation_size, GFP_KERNEL);
    if (name == NULL)
    {
        return NULL;
    }
    memset(name, 0, allocation_size - 1);
    strcpy(name, "kotp_");
    strcat(name, method_name);
    return name;
}

static int init_otp_method(OtpMethod_t *method)
{
    const char *device_name = NULL;
    int error = 0;

    pr_info("kotp: Initializing %s method\n", method->name);
    if (method->init() != 0)
    {
        pr_info("kotp: %s init method returned non zero status.\n", method->name);
        return -1;
    }
    method->write_fops(&method->fops);
    device_name = create_device_name(method->name);
    if (device_name == NULL)
    {
        pr_err("kotp: Failed device name memory allocation for method %s.\n", method->name);
        return -1;
    }
    method->misc_device.fops = &method->fops;
    method->misc_device.minor = MISC_DYNAMIC_MINOR;
    method->misc_device.name = device_name;
    method->misc_device.mode = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR;
    error = misc_register(&method->misc_device);
    if (error)
    {
        pr_err("kotp: Failed to register misc device for method %s.\n", method->name);
        return -1;
    }
    method->is_init = true;
    return 0;
}

static int __init otp_init(void)
{
    pr_info("kotp: Loading module\n");

    for (int i = 0; i < otp_amount; i++) {
        if (init_otp_method(&methods[i]) != 0) {
            pr_info("kotp: Failed loading %s method.\n", methods[i].name);
        }
        else {
            pr_info("kotp: Loaded %s method succesfully.\n", methods[i].name);
        }
    }
    pr_info("kotp: Module loaded\n");
    return 0;
}

static void __exit otp_exit(void)
{
    pr_info("kotp: Unloading module.\n");
    for (int i = 0; i < otp_amount; i++) {
        if (methods[i].is_init) {
            pr_info("kotp: Unloading method %s.\n", methods[i].name);
            misc_deregister(&methods[i].misc_device);
            methods[i].clean();
            if (methods[i].misc_device.name != NULL)
            {
                kfree(methods[i].misc_device.name);
            }
            pr_info("kotp: %s method unloaded.\n", methods[i].name);
        }
    }
    pr_info("kotp: Module unloaded.\n");
}

module_init(otp_init);
module_exit(otp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("A simple OTP Kernel Module");
