#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h> //For kernel module programming like module_init
                          //and module_exit etc
#include <asm/io.h>
#include <linux/cdev.h> //For cdev structure
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h> //For file structure
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kdev_t.h> //For device files
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/slab.h> //For kmalloc()
#include <linux/spinlock.h>
#include <linux/sysfs.h>
#include <linux/timer.h>
#include <linux/uaccess.h> //For function calls like copy_to/from_user()
#include <linux/wait.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#include <linux/sched/signal.h>
#endif

int delay = HZ;

static struct task_struct *thread_wait;

static int thread_func(void *wait) {
  unsigned long jfs1, jfs2;

  allow_signal(SIGKILL);

  while (!kthread_should_stop()) {

    jfs1 = jiffies;
    jfs2 = jfs1 + delay;

    while (time_before(jiffies, jfs2)) {
      schedule();
    }

    jfs2 = jiffies;

    printk(KERN_INFO "Jiffies starts = %lu\t and jiffies ends  = %lu\n", jfs1,
           jfs2);

    if (signal_pending(current)) {
      break;
    }
  }

  printk(KERN_INFO "Thread is stopping...\n");
  thread_wait = NULL;
  // do_exit(0); //Should be here, but compiler does not recognize do_exit(),
  // and throw error
  return 0;
}

static int __init my_init(void) {
  printk(KERN_INFO "Creating thread...\n");
  thread_wait = kthread_run(thread_func, NULL, "my_thread");
  return 0;
}

static void __exit my_exit(void) {
  printk(KERN_INFO "Removing the module...\n");

  if (thread_wait != NULL) {
    kthread_stop(thread_wait);
    printk(KERN_INFO "Stopping the tread...\n");
  }
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("A. H.");
MODULE_DESCRIPTION("Schedule driver");
