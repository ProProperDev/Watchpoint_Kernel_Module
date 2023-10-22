#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/kthread.h>

static int value = 0;
static struct task_struct *test_wp_thread = NULL;

static int value_increment(void *data) {
    while (!kthread_should_stop()) {
        ++value;
        msleep (10000);
        pr_info("%s: Value is: %d\n", THIS_MODULE->name, value);
        pr_info("%s: Value address: 0x%px\n", THIS_MODULE->name, &value);
    }
    return 0;
}
   
       
static int __init testwp_init(void) {
    int err;
	
    pr_info("Loading module \"%s\"...\n", THIS_MODULE->name);
    pr_info("%s: Value address: 0x%px\n", THIS_MODULE->name, &value);

    test_wp_thread = kthread_run(value_increment, NULL, "test_wp_thread");

    if (IS_ERR(test_wp_thread)) { 
        pr_info("Unable to start kernel thread\n");
        err = PTR_ERR(test_wp_thread);
        test_wp_thread = NULL;
        return err;
    }

	return 0;
}
 
static void __exit testwp_exit(void) {
    kthread_stop(test_wp_thread);
	pr_info("Unloading module...\"%s\"\n", THIS_MODULE->name);
}

module_init(testwp_init);
module_exit(testwp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Proper Sergei, proper_dev@yahoo.com");
MODULE_DESCRIPTION("Test for watchpoint module");
