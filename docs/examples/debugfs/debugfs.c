#include <linux/kernel.h> /* Needed for pr_info() */
#include <linux/module.h> /* Needed by all modules */
#include <linux/debugfs.h> /* Needed for debug filesystem */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("flav");
MODULE_DESCRIPTION("debugfs file");

#define LKMPG_TST_DFS "lkmpg_dfs"

static int tst_val_0 = 1;
static struct dentry *d_debugfs;

static int __init debugfs_init(void)
{
    pr_info("INIT: debugfs lkmpg example\n");

    /* Create entry at the root of debugfs */
    d_debugfs = debugfs_create_dir(LKMPG_TST_DFS, NULL);
    if (d_debugfs == NULL) {
     pr_warn("debugfs_lkmpg: failed to create %s\n", LKMPG_TST_DFS);
     return -1;
    }

    debugfs_create_u32("TST_U32", S_IRUSR|S_IWUSR, d_debugfs, &tst_val_0);

    return 0;
}

static void __exit debugfs_exit(void)
{
    pr_info("EXIT: debugfs lkmpg example\n");

    /* remove entry recursive in debugfs
     * Note: You must be aware that there is no automatic cleanup.
     * SEE: Documentation/filesystem/debugfs.rst
     */
    pr_info("val: %d\n", tst_val_0);
    debugfs_remove_recursive(d_debugfs);
}
 