#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h> //For kernel module programming like module_init
                         //and module_exit etc
#include<linux/kdev_t.h> //For device files
#include<linux/fs.h> //For file structure
#include<linux/cdev.h> //For cdev structure
#include<linux/device.h> 
#include<linux/slab.h> //For kmalloc()
#include<linux/uaccess.h> //For function calls like copy_to/from_user()

#define mem_size 1024

dev_t dev = 0; //initialize dev
static struct class *dev_class;
static struct cdev my_cdev;
uint8_t *kernel_buffer;

static int __init chr_driver_init(void); //First we write the init function
static void __exit chr_driver_exit(void);
static int chr_open(struct inode *inode, struct file *file);
static int chr_release(struct inode *inode, struct file *file);
static ssize_t chr_read(struct file *filp, char __user *buf, size_t len,
        loff_t * off);
static ssize_t chr_write(struct file *filp, const char *buf, size_t len,
        loff_t * off);

static struct file_operations fops =   //Create file operation structure 
{
    .owner     = THIS_MODULE,
    .read      = chr_read,
    .write     = chr_write,
    .open      = chr_open,
    .release   = chr_release,
};

static int chr_open(struct inode *inode, struct file *file)
{
    //Creating physical memory
    if ((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0)
    {
        printk(KERN_INFO"Cannot allocate the memory to the kernel...\n");
        return -1;
    }

    printk(KERN_INFO"Device file opened...\n");
    return 0;
}

static int chr_release(struct inode *inode, struct file *file)
{
    kfree(kernel_buffer);
    printk(KERN_INFO"Device file closed...\n");
    return 0;
}

static ssize_t chr_read(struct file *filp, char __user *buf, size_t len, 
        loff_t *off)
{
    if (copy_to_user(buf, kernel_buffer, mem_size) < 0)
    {
        printk(KERN_INFO"Cannot read data...\n");
        return -1;
    }
    printk(KERN_INFO"Data read: Done...\n");
    return mem_size; 
}

static ssize_t chr_write(struct file *filp, const char __user *buf, 
        size_t len, loff_t *off)
{
    if (copy_from_user(kernel_buffer, buf, len) < 0)
    {
        printk(KERN_INFO"Cannot write data...\n");
        return -1;
    }
    printk(KERN_INFO"Data is written successfully...\n");
    return len; 
}

static int __init chr_driver_init(void)
{
    //First allocating major number dynamically
    if ((alloc_chrdev_region(&dev,0,1,"my_Dev")) < 0)
    {
        printk(KERN_INFO"Cannot allocate the major number...\n");
        return -1;
    }

    printk(KERN_INFO"Major = %d Minor = %d..\n", MAJOR(dev), MINOR(dev));

    //Creating cdev structure; if we want to open, read, write, close a
    //file, we need to write a file operation structure in Kernel driver
    cdev_init(&my_cdev, &fops);
    
    //Adding character device to the system
    if ((cdev_add(&my_cdev, dev, 1)) < 0) 
    {
        printk(KERN_INFO"Cannot add the device to the system...\n");
        goto r_class;
    }
    
    //Creating struct class
    if ((dev_class = class_create("my_class")) == NULL) //((dev_class = class_create(THIS_MODULE, "my_class")) == NULL)
    {
        printk(KERN_INFO"Cannot create the struct class...\n");
        goto r_class;
    }

    //Creating device
    if ((device_create(dev_class, NULL, dev, NULL, "my_device")) == NULL)
    {
        printk(KERN_INFO"Cannot create the device...\n");
        goto r_device;
    }
    
    //Device driver has inserted properly
    printk(KERN_INFO"Device driver insert...done properly...\n");
    return 0;

r_device:
    //Destroy device since class created, but device not created
    class_destroy(dev_class);

r_class:
    //Unregister character device region alocated by alloc_chrdev_region
    unregister_chrdev_region(dev, 1);
    return -1;
}

//Remove the device driver totally
void __exit chr_driver_exit(void)
{
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO"Device driver is removed successfully...\n");
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("A. H.");
MODULE_DESCRIPTION("The character device driver");
