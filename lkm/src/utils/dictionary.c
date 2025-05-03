#include "dict.h"

static dictionary_t *head = NULL;
static int next_id = 1;
static int length = 0;

/**
 * Add a word to the dictionary
 * This word will be used to create new password combo
 * return 0 if it worked
 * return 1 if error
 */
int dictionary_add_word(const char *word)
{
    dictionary_t *new_node = kmalloc(sizeof(dictionary_t), GFP_KERNEL);
    if (!new_node)
    {
        pr_err("kotp: Memory allocation failed for new dictionary node\n");
        return 1;
    }

    new_node->id = next_id;
    next_id = next_id + 1;
    new_node->word = kstrdup(word, GFP_KERNEL);
    if (!new_node->word)
    {
        pr_err("kotp: Memory allocation failed for new dictionary word\n");
        kfree(new_node);
        return 1;
    }
    length = length + 1;
    new_node->next = NULL;

    if (head == NULL)
    {
        head = new_node;
    }
    else
    {
        dictionary_t *temp = head;

        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
    return 0;
}

/**
 * Delete a word from the dictionary
 * it wont be available to create new password
 * return 0 if a password is deleted
 * return 1 if nothing happened
 */
int dictionary_remove_word(int id)
{
    if (head == NULL)
    {
        return 1;
    }

    dictionary_t *temp = head;
    dictionary_t *prev = NULL;

    if (temp != NULL && temp->id == id)
    {
        head = temp->next;
        kfree(temp->word);
        kfree(temp);
        length = length - 1;
        return 0;
    }
    while (temp != NULL && temp->id != id)
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL)
    {
        return 1;
    }
    prev->next = temp->next;
    kfree(temp->word);
    kfree(temp);
    length = length - 1;
    return 0;
}

/**
 * Returns a node from its name
 * operztion should be readonly
 */
dictionary_t *find_node_by_word(const char *word)
{
    dictionary_t *temp = head;

    while (temp != NULL)
    {
        if (strcmp(temp->word, word) == 0)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

/**
 * Return the whole dictionary
 * Operation should only be read
 */
dictionary_t *get_dictionary()
{
    return head;
}

/**
 * Return the length of the dictionary
 */
int get_dictionary_length()
{
    return length;
}

/**
 * Return the node at an offset
 * Null if offset is invalid
 */
dictionary_t *get_dictionary_word_at(int position)
{
    if (position < 0)
    {
        return NULL;
    }
    dictionary_t *temp = head;

    for (int i = 0; i < position; i++)
    {
        if (temp == NULL)
        {
            return NULL;
        }
        temp = temp->next;
    }
    return temp;
}

/**
 * Delete the dictionary, useful for clean up
 */
void delete_dictionary()
{
    dictionary_t *tmp = NULL;

    if (head == NULL)
    {
        return;
    }
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        kfree(tmp->word);
        kfree(tmp);
    }
    return;
}