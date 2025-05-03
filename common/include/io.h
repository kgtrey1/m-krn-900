#ifndef OTP_IO_H
#define OTP_IO_H

#define MAX_BUFFER_SIZE 512 

typedef struct data_transfer_s
{
    void *data;
    long size;
    char resp[MAX_BUFFER_SIZE];
} data_transfer_t;

#define VERIFY_PASSWORD_CMD _IOW('V', 1, data_transfer_t)

/* Dict space */
#define DICT_ADD _IOW('A', 2, data_transfer_t)
#define DICT_DELETE _IOW('D', 3, data_transfer_t)
#define DICT_CONFIG _IOW('C', 4, data_transfer_t)
#define DICT_TIME _IOW('T', 5, data_transfer_t)

/* Time space */
#define TIME_KEY _IOW('K', 2, data_transfer_t)
#define TIME_CONFIG _IOW('C', 3, data_transfer_t)
#define TIME_TIME _IOW('T', 4, data_transfer_t)

#endif /* OTP_IO_H */
