#ifndef OTP_DICT_H
#define OTP_DICT_H

#include <linux/fs.h>

void dict_get_fops(struct file_operations *fops);
ssize_t dict_clean(void);
ssize_t dict_init(void);

typedef struct dictionary {
    int id;
    char *word;
    struct dictionary *next;
} dictionary_t;

void delete_dictionary(void);
int dictionary_add_word(const char *word);
int dictionary_remove_word(int id);
int get_dictionary_length(void);
dictionary_t *find_node_by_word(const char *word);
dictionary_t *get_dictionary(void);
dictionary_t *get_dictionary_word_at(int position);

#endif /* OTP_DICT_H */