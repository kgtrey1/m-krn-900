#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "lib_kotp.h"

void free_otp_methods_list(otp_method_list_t *list)
{
    otp_method_list_t *tmp = NULL;

    if (list == NULL)
    {
        return;
    }
    while (list != NULL)
    {
        tmp = list;
        list = list->next;
        free(tmp->name);
        free(tmp);
    }
    return;
}

static otp_method_list_t *create_node(const char *name)
{
    otp_method_list_t *node = NULL;

    node = malloc(sizeof(otp_method_list_t));
    if (node == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    node->name = strdup(name);
    if (node->name == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(node);
        return NULL;
    }
    node->next = NULL;
    return node;
}

static otp_method_list_t *add_otp_method(otp_method_list_t *list, const char *name)
{
    otp_method_list_t *tmp = list;

    if (tmp == NULL)
    {
        tmp = create_node(name);
        return tmp;
    }
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = create_node(name);
    if (tmp->next == NULL)
    {
        free_otp_methods_list(list);
        return NULL;
    }
    return list;
}


otp_method_list_t *get_available_otp_methods(int *status)
{
    DIR *dir = opendir("/dev");
    struct dirent *entry;
    char *occurence = NULL;
    otp_method_list_t *list = NULL;

    if (dir == NULL)
    {
        perror("Error");
        return NULL;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        occurence = strstr(entry->d_name, "kotp_");
        if (occurence == entry->d_name)
        {
            list = add_otp_method(list, entry->d_name + 5);
            if (list == NULL)
            {
                *status = -1;
                closedir(dir);
                return NULL;    
            }
        }
    }
    closedir(dir);
    return list;
}