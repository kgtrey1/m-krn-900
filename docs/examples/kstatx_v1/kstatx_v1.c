#include <linux/init.h>
#include <linux/kernel.h> /* for ARRAY_SIZE() */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/stat.h>
#include <linux/sched.h> 
#include <asm/uaccess.h> 
#include <linux/kernel.h>
#include <linux/kprobes.h> 
#include <linux/unistd.h> 

MODULE_VERSION("v.0");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flavien <flav> ASTRAUD <flav@epita.fr>");
MODULE_DESCRIPTION("ex. pour interception d'un syscall");

#define SYSCALL_SYM	"sys_call_table"
static unsigned long **syscall_table;

/* the write_cr0 function cannot be used because of the sensitive cr0 bits pinned by the security issue */
static inline void __write_cr0(unsigned long cr0) 
{ 
	asm volatile("mov %0,%%cr0" : "+r"(cr0) : : "memory"); 
}

static asmlinkage long (*ori_statx)(const struct pt_regs *);

static long m_statx(const struct pt_regs *regs)
{
	char tmp[128];
	int r = 0;

//	struct statx *statxbuf = (struct statx *) regs->r8;
	
	r = strncpy_from_user(tmp, (char __user *)regs->si, 128);
	pr_info("STATX: (%d) %s\n", r, tmp);

	return ori_statx(regs);
}

static int override_statx(void)
{
	unsigned long cr0;
	unsigned long (*kallsyms_lookup_name)(const char *name); 

	struct kprobe kp = { 
		.symbol_name = "kallsyms_lookup_name", 
	};
 
	if (register_kprobe(&kp) < 0) 
		return 0; 
	kallsyms_lookup_name = (unsigned long (*)(const char *name))kp.addr; 
	unregister_kprobe(&kp); 
	
	syscall_table = (unsigned long **) 
		kallsyms_lookup_name(SYSCALL_SYM);
	pr_info("sym addr : [%pK]\n", syscall_table);

	if (syscall_table == NULL) {
		pr_warn("AIE !!!\n");
	}

	cr0 = read_cr0();
	__write_cr0(cr0 & ~X86_CR0_WP);
	
	ori_statx = (void *)syscall_table[__NR_statx];
	syscall_table[__NR_statx] = (unsigned long *)m_statx;
	pr_info("takeover: statx\n");

	__write_cr0(cr0);
	return 0;
}

static void pullback_syscall(void)
{
	unsigned long cr0;

	cr0 = read_cr0();
	__write_cr0(cr0 & ~X86_CR0_WP);

	syscall_table[__NR_statx] = (unsigned long *)ori_statx;
	pr_info("getback : statx\n");

	__write_cr0(cr0);

	return;
}

static int __init kstatx_init(void)
{
	pr_info("kstatx INIT\n=============\n");

	override_statx();

	return 0;
}

static void __exit kstatx_exit(void)
{
        pr_info("kstatx END\n=============\n");	
	
	pullback_syscall();
}

module_init(kstatx_init);
module_exit(kstatx_exit);
