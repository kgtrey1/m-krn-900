#include <linux/types.h>
#include <linux/delay.h>
#include "kotp.h"
#include "time.h"
#include "io.h"

static int expiration_time = 0;
static const char otp_method_name[] = "time";
static char secret_key[MAX_KEY_LENGTH];

static long key(data_transfer_t *arg)
{
    if (arg->data == NULL)
    {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Error: Arguments are invalid\n");
        return 1;
    }
    int length = strlen(arg->data);

    if (length > 32 || length < 3)
    {
        return -1;
    }
    memset(secret_key, 0, MAX_KEY_LENGTH);
    strcpy(secret_key, arg->data);
    snprintf(arg->resp, MAX_BUFFER_SIZE, "Success: Secret key changed successfuly.\n");
    return 0;
}

static long config(data_transfer_t *arg)
{
    snprintf(arg->resp, MAX_BUFFER_SIZE, "Expiration time: %d\nSecret key: %s\n", expiration_time, secret_key);
    return 0;
}

static long time(data_transfer_t *arg)
{
    if (arg->data == NULL)
    {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Error: Arguments are invalid\n");
        return 1;
    }
    int result = 0;
    int ret = kstrtoint(arg->data, 10, &result);

    if (ret)
    {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Error: Failed to convert argument\n");
        return 1;
    }
    else
    {
        expiration_time = result;
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Success: Expiration is set to %d seconds\n", result);
        return 0;
    }
}

static long consume(data_transfer_t *arg)
{
    if (arg->data == NULL) {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Error: Arguments looks invalid\n");
        return 1;
    }
    size_t res = consume_password(otp_method_name, (char *)arg->data);

    if (res == OTP_CONSUME_SUCCESS) {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Success: Password is correct\n");
        return 0;
    }
    else if (res == OTP_CONSUME_EXPIRED)
    {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Error: Password has expired\n");
        return 1;
    }
    snprintf(arg->resp, MAX_BUFFER_SIZE, "Error: Invalid password\n");
    return 1;
}

static void generate_otp(char *buf) {
    unsigned long current_time = 0;
    struct timespec64 ts;
    unsigned int key_sum = 0;
    unsigned long otp_val;

    ktime_get_real_ts64(&ts);
    current_time = ts.tv_nsec; 
    for (int i = 0; secret_key[i] != '\0'; i++) {
        key_sum ^= secret_key[i];
    }
    otp_val = (current_time + key_sum) % 1000000;
    snprintf(buf, MAX_TIME_PASSWORD_LENGTH, "%06lu", otp_val);
}

static ssize_t otp_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{        
    /**
     * From what I understand, cat does multiple small reads. Our passwords are rather short.
     * This mean we need only one read to finish writting them. When we do our first read, we
     * modify the offset. If the offset is not 0, then it means we already sent data. We then
     * reset the offset to 0 and trigger an EOF so cat stop reading. 
     */
    if (*offset > 0)
    {
        *offset = 0;
        return 0;
    }

    char pw_buf[MAX_TIME_PASSWORD_LENGTH];

    generate_otp(pw_buf);
    if (expiration_time == 0)
    {
        create_password(otp_method_name, pw_buf, true, 0);
    }
    else
    {
        struct timespec64 ts;
        ktime_get_real_ts64(&ts);
        create_password(otp_method_name, pw_buf, true, ts.tv_sec + expiration_time);
    }
    snprintf(pw_buf, MAX_TIME_PASSWORD_LENGTH, "%s\n", pw_buf);

    size_t string_len = strlen(pw_buf);
    size_t to_copy = min(len, string_len);
    
    if (copy_to_user(buf, pw_buf, to_copy))
    {
        return -EFAULT;
    }
    *offset += to_copy;    
    return to_copy;
}

static long otp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long status = 0;
    data_transfer_t user_data;

    if (copy_from_user(&user_data, (data_transfer_t *)arg, sizeof(data_transfer_t)))
    {
        return -EFAULT;
    }
    switch (cmd)
    {
        case VERIFY_PASSWORD_CMD:
            status = consume(&user_data);
            break;
        case TIME_TIME:
            status = time(&user_data);
            break;
        case TIME_CONFIG:
            status = config(&user_data);
            break;
        case TIME_KEY:
            status = key(&user_data);
            break;
        default:
            sprintf(user_data.resp, "Error: Unknown command\n");
            status = -EINVAL;
            break;
    }
    if (copy_to_user((data_transfer_t *)arg, &user_data, sizeof(data_transfer_t)))
    {
        return -EFAULT;
    }
    return status;
}

void totp_get_fops(struct file_operations *fops)
{
    fops->owner = THIS_MODULE;
    fops->read = otp_read;
    fops->unlocked_ioctl = otp_ioctl;
    return;
}

ssize_t totp_clean()
{
    return 0;
}

ssize_t totp_init()
{
    strcpy(secret_key, "ChangeMe");
    return 0;
}