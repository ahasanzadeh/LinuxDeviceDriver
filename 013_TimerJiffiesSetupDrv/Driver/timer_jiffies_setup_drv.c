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
#include <linux/slab.h> //For kmalloc()
#include <linux/sysfs.h>
#include <linux/timer.h>
#include <linux/uaccess.h> //For function calls like copy_to/from_user()

#define TIMEOUT 5000 /* millisecond */
#define IRQ_NO 1

unsigned int i = 0;

void tasklet_func(unsigned long);

struct tasklet_struct *tasklet;

unsigned long chr_spinlock_variable = 0;

static struct timer_list chr_timer;

// static struct task_struct *chr_thread; /* we need 2 thrads for this
// example; see lines 57-58 below */

/* Declare the tasklet for static method */
// DECLARE_TASKLET(tasklet, tasklet_func, 1); /* Data is 1 */

DEFINE_SPINLOCK(chr_spinlock);

/* Timer callback function which is called when timer expires */
void timer_callback(struct timer_list *timer) {
  printk(KERN_INFO "in timer callback function[%d]\n", i++);
  /* Reenable the timer which will make this as predict timer */
  mod_timer(&chr_timer, msecs_to_jiffies(TIMEOUT));
}

/* Tasklet function body */
void tasklet_func(unsigned long data) {
  spin_lock_irq(&chr_spinlock);
  chr_spinlock_variable++;
  printk(KERN_INFO "Executing the tasklet function: data = %lu\n",
         chr_spinlock_variable);
  spin_unlock_irq(&chr_spinlock);
}

// Interrupt handler for IRQ 1
static irqreturn_t irq_handler(int irq, void *dev_id) {
  spin_lock_irq(&chr_spinlock);
  chr_spinlock_variable++;
  printk(KERN_INFO "Interrupt Occured and eecuting ISR routine... %lu\n",
         chr_spinlock_variable);
  spin_unlock_irq(&chr_spinlock);
  /* Scheduling the tasklet */
  tasklet_schedule(tasklet);
  return IRQ_HANDLED;
}

// volatile int chr_value = 0;

dev_t dev = 0; // initialize dev
static struct class *dev_class;
static struct cdev my_cdev;
struct kobject *kobj_ref;

static int __init chr_driver_init(void); // First we write the init function
static void __exit chr_driver_exit(void);

static struct task_struct *chr_thread1;
static struct task_struct *chr_thread2;

/******************** Driver Functions ********************/
static int chr_open(struct inode *inode, struct file *file);
static int chr_release(struct inode *inode, struct file *file);
static ssize_t chr_read(struct file *filp, char __user *buf, size_t len,
                        loff_t *off);
static ssize_t chr_write(struct file *filp, const char *buf, size_t len,
                         loff_t *off);
int thrd_func1(void *p);
int thrd_func2(void *p);

int thrd_func1(void *p) {
  // int i = 0;
  while (!kthread_should_stop()) {
    if (!spin_is_locked(&chr_spinlock)) {
      printk(KERN_INFO "Spinlock is not locked in thread function 1...\n");
    }
    spin_lock(&chr_spinlock);
    if (spin_is_locked(&chr_spinlock)) {
      printk(KERN_INFO "Spinlock is locked in thread function 1...\n");
    }
    chr_spinlock_variable++;
    printk(KERN_INFO "In thread function 1 %lu\n", chr_spinlock_variable);
    spin_unlock(&chr_spinlock);
    msleep(1000);
  }
  return 0;
}

int thrd_func2(void *p) {
  // int i = 0;
  while (!kthread_should_stop()) {
    if (!spin_is_locked(&chr_spinlock)) {
      printk(KERN_INFO "Spinlock is not locked in thread function 2...\n");
    }
    spin_lock(&chr_spinlock);
    if (spin_is_locked(&chr_spinlock)) {
      printk(KERN_INFO "Spinlock is locked in thread function 2...\n");
    }
    chr_spinlock_variable++;
    printk(KERN_INFO "In thread function 2 %lu\n", chr_spinlock_variable);
    spin_unlock(&chr_spinlock);
    msleep(1000);
  }
  return 0;
}

static struct file_operations fops = // Create file operation structure
    {
        .owner = THIS_MODULE,
        .read = chr_read,
        .write = chr_write,
        .open = chr_open,
        .release = chr_release,
};

static int chr_open(struct inode *inode, struct file *file) {
  printk(KERN_INFO "Device file opened...\n");
  return 0;
}

static int chr_release(struct inode *inode, struct file *file) {
  printk(KERN_INFO "Device file closed...\n");
  return 0;
}

static ssize_t chr_read(struct file *filp, char __user *buf, size_t len,
                        loff_t *off) {
  printk(KERN_INFO "Read function...\n");
  // asm("int $0x3B");  /* corresponding to irq 1 */
  return 0;
}

static ssize_t chr_write(struct file *filp, const char __user *buf, size_t len,
                         loff_t *off) {
  printk(KERN_INFO "write function...\n");
  return 0;
}

static int __init chr_driver_init(void) {
  // First allocating major number dynamically
  if ((alloc_chrdev_region(&dev, 0, 1, "my_Dev")) < 0) {
    printk(KERN_INFO "Cannot allocate the major number...\n");
    return -1;
  }

  printk(KERN_INFO "Major = %d Minor = %d..\n", MAJOR(dev), MINOR(dev));

  // Creating cdev structure; if we want to open, read, write, close a
  // file, we need to write a file operation structure in Kernel driver
  cdev_init(&my_cdev, &fops);

  // Adding character device to the system
  if ((cdev_add(&my_cdev, dev, 1)) < 0) {
    printk(KERN_INFO "Cannot add the device to the system...\n");
    goto r_class;
  }

  // Creating struct class
  if ((dev_class = class_create("my_class")) ==
      NULL) //((dev_class = class_create(THIS_MODULE, "my_class")) == NULL)
  {
    printk(KERN_INFO "Cannot create the struct class...\n");
    goto r_class;
  }

  // Creating device
  if ((device_create(dev_class, NULL, dev, NULL, "my_device")) == NULL) {
    printk(KERN_INFO "Cannot create the device...\n");
    goto r_device;
  }

  /* Interrupt */
  if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "my_device",
                  (void *)(irq_handler))) {
    printk(KERN_INFO "my_device: cannot register IRQ...\n");
    goto irq;
  }

  /* Setup the timer to call timer callback fucntion */
  timer_setup(&chr_timer, timer_callback, 0);

  /* Setup the timer interval to base on TIMEOUT macro */
  mod_timer(&chr_timer, jiffies + msecs_to_jiffies(TIMEOUT));

  tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
  if (tasklet == NULL) {
    printk(KERN_INFO "cannot allocate the memory...\n");
    goto irq;
  }
  tasklet_init(tasklet, tasklet_func, 0); /* Dynamic method */

  chr_thread1 = kthread_create(thrd_func1, NULL, "chr thread 1");
  if (chr_thread1) {
    wake_up_process(chr_thread1);
  } else {
    printk(KERN_INFO "Unable to create the thread 1...\n");
    goto r_device;
  }

  chr_thread2 = kthread_run(thrd_func2, NULL, "chr_thread 2");
  if (chr_thread2) {
    printk(KERN_INFO "Successfully created the kernel thread 2...\n");
  } else {
    printk(KERN_INFO "Unable to create the thread 2...\n");
    goto r_device;
  }

  printk(KERN_INFO "Device driver insert...done properly...\n");
  return 0;

irq:
  free_irq(IRQ_NO, (void *)(irq_handler));

r_device:
  // Destroy device since class created, but device not created
  class_destroy(dev_class);

r_class:
  // Unregister character device region alocated by alloc_chrdev_region
  unregister_chrdev_region(dev, 1);
  return -1;
}

// Remove the device driver totally
void __exit chr_driver_exit(void) {
  kthread_stop(chr_thread1);
  kthread_stop(chr_thread2);
  free_irq(IRQ_NO, (void *)(irq_handler));
  tasklet_kill(tasklet);
  del_timer(&chr_timer);
  device_destroy(dev_class, dev);
  class_destroy(dev_class);
  cdev_del(&my_cdev);
  unregister_chrdev_region(dev, 1);
  printk(KERN_INFO "Device driver is removed successfully...\n");
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("A. H.");
MODULE_DESCRIPTION("The character device driver");
