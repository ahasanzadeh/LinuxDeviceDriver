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
#include <linux/mutex.h>
#include <linux/slab.h> //For kmalloc()
#include <linux/sysfs.h>
#include <linux/timer.h>
#include <linux/uaccess.h> //For function calls like copy_to/from_user()

struct mutex chr_mutex;
unsigned long chr_mutex_var = 0;

dev_t dev = 0; // initialize dev
static struct class *dev_class;
static struct cdev my_cdev;

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
  while (!kthread_should_stop()) {
    mutex_lock(&chr_mutex);
    chr_mutex_var++;
    printk(KERN_INFO "In thread function 1 %lu\n", chr_mutex_var);
    mutex_unlock(&chr_mutex);
    msleep(1000);
  }
  return 0;
}

int thrd_func2(void *p) {
  while (!kthread_should_stop()) {
    mutex_lock(&chr_mutex);
    chr_mutex_var++;
    printk(KERN_INFO "In thread function 2 %lu\n", chr_mutex_var);
    mutex_unlock(&chr_mutex);
    msleep(1000);
  }
  return 0;
}

/******************** File Operation Structure ********************/
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = chr_read,
    .write = chr_write,
    .open = chr_open,
    .release = chr_release,
};

/******************** Function Definition ********************/
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
  return len;
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

  /*Initializing the mutex in dynamic method */
  mutex_init(&chr_mutex);

  /* Creating thread 1 to test the mutex */
  chr_thread1 = kthread_run(thrd_func1, NULL, "Mutex Thread 1");
  if (chr_thread1) {
    printk(KERN_ERR "Created kernek thread 1 successfully\n");
  } else {
    printk(KERN_ERR "Unable to create the kernel thread 1...\n");
    goto r_device;
  }

  /* Creating thread 2 to test the mutex */
  chr_thread2 = kthread_run(thrd_func2, NULL, "Mutex Thread 2");
  if (chr_thread2) {
    printk(KERN_ERR "Created kernek thread 2 successfully\n");
  } else {
    printk(KERN_ERR "Unable to create the kernel thread 2...\n");
    goto r_device;
  }

  printk(KERN_INFO "Inserting the character Device driver...\n");
  return 0;

r_device:
  // Destroy device since class created, but device not created
  class_destroy(dev_class);

r_class:
  // Unregister character device region alocated by alloc_chrdev_region
  unregister_chrdev_region(dev, 1);
  cdev_del(&my_cdev);
  return -1;
}

// Remove the device driver totally
void __exit chr_driver_exit(void) {
  kthread_stop(chr_thread1);
  kthread_stop(chr_thread2);
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
MODULE_DESCRIPTION("Mutex device driver");
