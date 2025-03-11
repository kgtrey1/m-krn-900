#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Create a device name from user input.
 * Do not forget to free it after.
 */
char *create_device_name(const char *method)
{
    const size_t allocation_size = sizeof(char) * (strlen(method) + strlen("/dev/kotp_") + 1);
    char *device = NULL;

    device = malloc(allocation_size);
    if (device == NULL)
    {
        perror("Error: Memory allocation failed.");
        return NULL;
    }
    memset(device, 0, allocation_size);
    strcpy(device, "/dev/kotp_");
    strcat(device, method);
    return device;
}