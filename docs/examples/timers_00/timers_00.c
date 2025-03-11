#include <linux/module.h>
#include <linux/printk.h>


#define DELAY_S 2 * HZ

static struct timer_list ftimers;
int n = 0;

struct timespec64 ts;


void ftimer_cb(struct timer_list *d)
{
	long unsigned int j = 0;
	long unsigned int f = 0;
	n++;

	ts.tv_sec = 5;
	ts.tv_nsec = 0;
	f = timespec64_to_jiffies(&ts);
	j = jiffies;
	pr_info("[%lld/%ld] [%lu] [%lu]\n", ts.tv_sec, ts.tv_nsec,
		j, f);
	ftimers.expires = j+f;
	add_timer(&ftimers);
}

int init_module(void) 
{ 
	pr_info("deb timers 00\n"); 
	
	timer_setup(&ftimers, ftimer_cb, 0);

	ktime_get_real_ts64(&ts);
	ftimers.expires = jiffies + DELAY_S;
	add_timer(&ftimers);
	return 0; 
} 

void cleanup_module(void) 
{ 
	del_timer(&ftimers);
	pr_info("fin timers 00.\n"); 
} 

MODULE_LICENSE("GPL");
