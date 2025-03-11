/**
 * kernel dev intro EPITECH
 * exemple procfs
 * @2023
 * flav <flav@epita.fr>
 **/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/version.h>

#define PROC_NAME "kdev_tst"

static struct proc_dir_entry *kdev_tst_proc_file;

static char data[1024];
static int data_len = 1024;

static ssize_t procfile_read(struct file *fp, char __user *buf,
			     size_t len, loff_t *offset)
{
	size_t ret;
	loff_t pos = *offset;

	pr_info("read[len:%ld / off:%lld]\n", len, *offset);

	if (pos < 0) {
		return -EINVAL;
	}

	if (pos >= data_len || !len) {
		pr_info("read: fini\n");
		return 0;
	}

	if (len > data_len - pos)
		len = data_len - pos;
	
	ret = copy_to_user(buf, data, len);
	pr_info("read: (%ld) data=[%s]\n", ret, data);
	
	if (ret == len) 
		return -EFAULT;
	
	len -= ret;
	*offset = pos + len;
	return (len);
}

static ssize_t procfile_write(struct file *fp, const char __user *buf,
			      size_t buf_len, loff_t *offset)
{
	loff_t pos = *offset;
	size_t res;

	pr_info("write[data_len: %d / len:%ld / off:%lld]\n", data_len,
		buf_len, *offset);

	if (pos < 0)
		return -EINVAL;
	if (pos >= data_len || !buf_len)
		return 0;
	if (buf_len > data_len - pos)
		buf_len = data_len - pos;
	res = copy_from_user(data + pos, buf, buf_len);
	if (res == buf_len)
		return -EFAULT;
	buf_len -= res;
	*offset = pos + buf_len;
	return buf_len;
}

/* static ssize_t procfile_write(struct file *fp, const char __user *buf, */
/* 			      size_t buf_len, loff_t *offset) */
/* { */
/* 	ssize_t ret; */

/* 	ret = simple_write_to_buffer(data, PAGE_SIZE, offset, buf, buf_len); */

/* 	data_len = ret; */

/* 	return buf_len; */
/* } */

static const struct proc_ops proc_file_fops = { 
	.proc_read = procfile_read, 
	.proc_write = procfile_write, 
};

static int __init kdev_procfs_init(void)
{
	pr_info("kdev procfs init\n");
	kdev_tst_proc_file = proc_create(PROC_NAME, 0644, NULL, &proc_file_fops);
	if (kdev_tst_proc_file == NULL) {
		proc_remove(kdev_tst_proc_file);
		pr_alert("Err: could not create /proc/%s\n", PROC_NAME);
		return -ENOMEM;
	}
	pr_info("kdev procfs create /proc/%s\n", PROC_NAME);
	return 0;
}

static void __exit kdev_procfs_exit(void)
{
	proc_remove(kdev_tst_proc_file);
	pr_info("kdev procfs remove /proc/%s\n", PROC_NAME);	
}

module_init(kdev_procfs_init);
module_exit(kdev_procfs_exit);

MODULE_LICENSE("GPL");
