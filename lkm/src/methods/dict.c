#include <linux/types.h>
#include <linux/delay.h>
#include "kotp.h"
#include "dict.h"
#include "io.h"

static int expiration_time = 0;
static const char otp_method_name[] = "dict";

static long add(data_transfer_t *arg)
{
    if (arg->data == NULL)
    {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Error: Arguments are invalid\n");
        return 1;
    }
    int length = strlen(arg->data);

    if (length > 16 || length < 3)
    {
        return -1;
    }
    if (find_node_by_word(arg->data) != NULL)
    {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Error: This word is already in the list.\n");
        return 1;
    }
    int res = dictionnary_add_word(arg->data);
    
    if (res == 0)
    {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Success: %s has been added to the dictionnary.\n", (char *)arg->data);
    }
    else
    {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Error: Failed to allocate memory.\n");
    }
    return (long)res;
}

static long delete(data_transfer_t *arg)
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
    if (dictionnary_remove_word(result) != 0)
    {
        snprintf(arg->resp, MAX_BUFFER_SIZE, "Error: Could not find a word associated to this id\n");
        return 1;
    }
    snprintf(arg->resp, MAX_BUFFER_SIZE, "Success: Word with id %d has been deleted\n", result);
    return 0;
}

static long config(data_transfer_t *arg)
{
    struct dictionnary *dict = get_dictionnary();
    struct dictionnary *temp = dict;
    int current_length = 0;

    snprintf(arg->resp, MAX_BUFFER_SIZE, "Expiration time: %d\nWordlist:\n", expiration_time);
    current_length = strlen(arg->resp);
    while (temp != NULL)
    {
        char line_buffer[64];

        memset(line_buffer, 0, 64);
        snprintf(line_buffer, 64, "%d: %s", temp->id, temp->word);
        if (strlen(line_buffer) + current_length <= MAX_BUFFER_SIZE - 1)
        {
            strcat(arg->resp, line_buffer);
            strcat(arg->resp, "\n");
            current_length = strlen(arg->resp);
        }
        temp = temp->next;
    }
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
    char pw_buf[128];
    int dictionnary_length = get_dictionnary_length();

    if (dictionnary_length < 2)
    {
        snprintf(pw_buf, 128, "Error: Not enough word to generate a password\n");
    }
    else
    {
        int idx1 = get_random_u32() % dictionnary_length;
        int idx2 = get_random_u32() % dictionnary_length;
    
        while (idx2 == idx1)
        {
            idx2 = get_random_u32() % dictionnary_length;
            msleep(1);
        }
        snprintf(pw_buf, 128, "%s%s", get_dictionnary_word_at(idx1)->word, get_dictionnary_word_at(idx2)->word);

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
        snprintf(pw_buf, 128, "%s\n", pw_buf);
    }    
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
        case DICT_ADD:
            status = add(&user_data);
            break;
        case DICT_DELETE:
            status = delete(&user_data);
            break;
        case DICT_CONFIG:
            status = config(&user_data);
            break;
        case DICT_TIME:
            status = time(&user_data);
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

void dict_get_fops(struct file_operations *fops)
{
    fops->owner = THIS_MODULE;
    fops->read = otp_read;
    fops->unlocked_ioctl = otp_ioctl;
    return;
}

ssize_t dict_clean()
{
    pr_info("kotp: cleaning up dictionnary");
    delete_dictionnary();
    return 0;
}

ssize_t dict_init()
{
    return 0;
}