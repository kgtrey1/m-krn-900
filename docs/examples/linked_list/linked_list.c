#include <linux/printk.h>
#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/slab.h> /* kmalloc */

MODULE_VERSION("v.0");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flavien <flav> ASTRAUD <flav@epita.fr>");
MODULE_DESCRIPTION("ex. liste chainee");

#define TST_DFS "lldfs"
static struct dentry *d_debugfs;
static struct dentry *data_file;

struct ts {
    struct list_head p_list;
    int idx;
    int m;
    int d;
    int dec;
};

LIST_HEAD(list);

static struct ts ts0 = {
    .idx = 0,
    .m = 10,
    .d = 1,
    .dec = 0,
};

static struct ts ts1 = {
    .idx = 1,
    .m = 10,
    .d = 1,
    .dec = 0,
};

static struct ts ts2 = {
    .idx = 2,
    .m = 20,
    .d = 1,
    .dec = 0,
};

static struct ts ts3 = {
    .idx = 3,
    .m = 30,
    .d = 1,
    .dec = 0,
};

char data[PAGE_SIZE];
int data_size = 0;

static ssize_t m_read(struct file *f, char *buffer, size_t len, loff_t *offset)
{
    struct ts *tmp;
    int c = 0;
    pr_info("m_read\n");

    list_for_each_entry(tmp, &list, p_list) {
        pr_info("%d-> %d:%d,%d,%d\n", c,
         tmp->idx,
         tmp->m,
         tmp->d,
         tmp->dec);
        c++;
    }

    return simple_read_from_buffer(buffer, len, offset, data, data_size);
}

static void add_in_list(struct ts *elm) {
    list_add(&elm->p_list, &list);
 
    pr_info("m_write-> ADD (%d):%d,%d,%d\n",elm->idx, elm->m, elm->d,
     elm->dec);
}
static void del_in_list(int pos) {
    pr_info("m_write-> DEL (%d)\n", pos);
}

static ssize_t m_write(struct file *f, const char *buffer, size_t len,
         loff_t *offset)
{
    ssize_t ret;
    int r = 0;
    char cmd[4];
    struct ts *tmp;

    ret = simple_write_to_buffer(data, PAGE_SIZE, offset, buffer, len);
    data_size = ret;
    pr_info("m_write: data size=%d\n", data_size);
    if (ret < 0)
     return ret;

    tmp = kmalloc(sizeof(*tmp), GFP_KERNEL);

    r = sscanf(data, "%s %d,%d,%d", cmd, &tmp->m, &tmp->d, &tmp->dec);
    pr_info("r==%d\n", r);
    if ((strcmp(cmd, "ADD") == 0) && (r == 4)) {
        add_in_list(tmp);
        goto out;
    }

    if ((strcmp(cmd, "DEL") == 0) && (r == 2)) {
        del_in_list(tmp->m);
        goto out;
    }

    return -EINVAL;

    out:
        return len;
}

const struct file_operations fops = {
    .write = m_write,
    .read = m_read,
};

static int __init linked_list_init(void)
{
    struct ts *tmp;
    int c = 0;

    pr_info("linked_list INIT\n=============\n");
 
    d_debugfs = debugfs_create_dir(TST_DFS, NULL);
    if (d_debugfs == NULL) {
        pr_info("creat : %s\n", TST_DFS);
        return -ENOENT;
    }

    data_file = debugfs_create_file("f0", 0660, d_debugfs, NULL, &fops);


    INIT_LIST_HEAD(&list);

    list_add(&ts2.p_list, &list);
    list_add(&ts0.p_list, &list);
    list_add(&ts3.p_list, &list);
    list_add(&ts1.p_list, &list);

    list_for_each_entry(tmp, &list, p_list) {
        pr_info("%d-> %d:%d,%d,%d\n", c,
            tmp->idx,
            tmp->m,
            tmp->d,
            tmp->dec);
        c++;
    }

    return 0;
}

static void __exit linked_list_exit(void)
{
    debugfs_remove_recursive(d_debugfs);
    pr_info("remove dfs\nlinked_list END\n=============\n"); 
}

module_init(linked_list_init);
module_exit(linked_list_exit);
