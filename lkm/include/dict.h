#ifndef OTP_DICT_H
#define OTP_DICT_H

#include <linux/fs.h>

void dict_get_fops(struct file_operations *fops);
ssize_t dict_clean(void);
ssize_t dict_init(void);

typedef struct dictionnary {
    int id;
    char *word;
    struct dictionnary *next;
} dictionnary_t;

void delete_dictionnary(void);
int dictionnary_add_word(const char *word);
int dictionnary_remove_word(int id);
int get_dictionnary_length(void);
dictionnary_t *find_node_by_word(const char *word);
dictionnary_t *get_dictionnary(void);
dictionnary_t *get_dictionnary_word_at(int position);

#endif /* OTP_DICT_H */