#include "kotp.h"

static OneTimePassword_t *head = NULL;

/**
 * Create a new password for an otp method
 * If validity is 0, it means that there is no expiration on this password
 */
static OneTimePassword_t* create_node(const char *method, const char *password, bool assigned, size_t validity)
{
    OneTimePassword_t *new_node = kmalloc(sizeof(OneTimePassword_t), GFP_KERNEL);

    if (!new_node)
    {
        pr_err("kotp: Failed to allocate memory for OneTimePassword\n");
        return NULL;
    }
    new_node->method = kstrdup(method, GFP_KERNEL);
    if (new_node->method == NULL)
    {
        kfree(new_node);
        pr_err("kotp: Failed to allocate memory for OneTimePassword\n");
        return NULL;
    }
    new_node->password = kstrdup(password, GFP_KERNEL);
    if (new_node->password == NULL)
    {
        kfree(new_node->method);
        kfree(new_node);
        pr_err("kotp: Failed to allocate memory for OneTimePassword\n");
        return NULL;
    }
    new_node->is_assigned = assigned;
    new_node->validity = validity;
    new_node->next = NULL;
    return new_node;
}

/* delete a node from its address */
static void delete_node(OneTimePassword_t* node)
{
    if (head == NULL)
    {
        return;
    }

    OneTimePassword_t *temp = head;
    OneTimePassword_t *prev = NULL;

    if (temp != NULL && temp == node)
    {
        head = temp->next;
        kfree(temp->method);
        kfree(temp->password);
        kfree(temp);
        return;
    }

    while (temp != NULL && temp != node)
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL)
    {
        return;
    }

    prev->next = temp->next;
    kfree(temp->method);
    kfree(temp->password);
    kfree(temp);
}

void create_password(const char *method, const char *password, bool assigned, size_t validity)
{
    OneTimePassword_t *new_node = create_node(method, password, assigned, validity);
    if (new_node == NULL)
    {
        pr_err("kotp: Failed to append OTP node\n");
        return;
    }

    if (head == NULL)
    {
        head = new_node;
    }
    else
    {
        OneTimePassword_t *temp = head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

/**
 * Returns 0 if everything went well
 * Returns 1 if the password is not found
 * Returns 2 if the password is expired
 */
size_t consume_password(const char *method, const char *password)
{
    if (head == NULL)
    {
        return OTP_CONSUME_NOT_FOUND;
    }

    OneTimePassword_t *temp = head;

    // If the head node itself holds the password
    if (temp != NULL && strcmp(temp->method, method) == 0 && strcmp(temp->password, password) == 0)
    {
        struct timespec64 ts;
        ktime_get_real_ts64(&ts);
        if (temp->validity != 0 && temp->validity < ts.tv_sec)
        {
            delete_node(temp);
            return OTP_CONSUME_EXPIRED;
        }
        delete_node(temp);
        return OTP_CONSUME_SUCCESS;
    }

    while (temp != NULL)
    {
        if  (strcmp(temp->method, method) == 0 && strcmp(temp->password, password) == 0)
        {
            break;
        }
        temp = temp->next;
    }

    if (temp == NULL)
    {
        return OTP_CONSUME_NOT_FOUND;
    }

    struct timespec64 ts;
    ktime_get_real_ts64(&ts);
    if (temp->validity != 0 && temp->validity < ts.tv_sec)
    {
        delete_node(temp);
        return OTP_CONSUME_EXPIRED;
    }
    delete_node(temp);
    return OTP_CONSUME_SUCCESS;
}

/**
 * Delete the dictionnary, useful for clean up
 */
void delete_password()
{
    OneTimePassword_t *tmp = NULL;

    if (head == NULL)
    {
        return;
    }
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        kfree(tmp->method);
        kfree(tmp->password);
        kfree(tmp);
    }
    return;
}