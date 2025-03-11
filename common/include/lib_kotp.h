#ifndef LIB_OTP_H
#define LIB_OTP_H

/**
 * OTP Method listing
 * get_available_otp_methods: returns a linked list of the availables OTP method.
 * free_otp_methods_list: will free the linked list.
 * Please, always call free_otp_methods_list on the head of the linked list
 * before exiting to prevent memory leaks.
 */

typedef struct otp_method_list_s
{
    char *name;
    struct otp_method_list_s *next;
} otp_method_list_t;

void free_otp_methods_list(otp_method_list_t *list);
otp_method_list_t *get_available_otp_methods(int *status);

/* device.c */
char *create_device_name(const char *method);

#endif /* LIB_OTP_H */