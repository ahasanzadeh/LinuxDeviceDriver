#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define DRIVER_NAME "Sample_Pldrv"
static unsigned int gpio_number = 0;

static dev_t first;
static struct cdev cdev;
static struct class *dev_class;

static int init_result;

/******************** Driver Functions ********************/
static int gpio_open(struct inode *inode, struct file *file);
static int gpio_close(struct inode *inode, struct file *file);
static ssize_t gpio_read(struct file *filp, char __user *buf, size_t len,
                         loff_t *off);
static ssize_t gpio_write(struct file *filp, const char *buf, size_t len,
                          loff_t *off);

/******************** File Operation Structure ********************/
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = gpio_read,
    .write = gpio_write,
    .open = gpio_open,
    .release = gpio_close,
};

/******************** Function Definition ********************/
static int gpio_open(struct inode *inode, struct file *file) { return 0; }

static int gpio_close(struct inode *inode, struct file *file) { return 0; }

static ssize_t gpio_read(struct file *filp, char __user *buf, size_t len,
                         loff_t *off) {
  unsigned char temp = gpio_get_value(gpio_number);

  if (copy_to_user(buf, &temp, 1)) {
    return -EFAULT;
  }

  return len;
}

static ssize_t gpio_write(
    struct file *filp, const char __user *buf, size_t len,
    loff_t *off) /*should __user be here? can I use count instead of len here?*/
{
  char temp;

  if (copy_from_user(&temp, buf, len)) {
    return -EFAULT;
  }

  printk(KERN_INFO "In write call...\n");

  switch (temp) {
  case '0':
    gpio_set_value(gpio_number, 0);
    break;
  case '1':
    gpio_set_value(gpio_number, 1);
    break;
  default:
    printk(KERN_INFO "Worng option is entered...\n");
    break;
  }

  return len;
}

static int sample_drv_probe(struct platform_device *pdev) {
  struct device_node *np = pdev->dev.of_node;
  printk(KERN_ALERT "In probe function call...\n");

  of_property_read_u32(np, "led-number", &gpio_number);

  init_result = alloc_chrdev_region(&first, 0, 1, "gpio_drv");
  if (init_result < 0) {
    printk(KERN_ALERT "Device registration failed...\n");
    return -1;
  }

  printk("Major number: %d\n", MAJOR(first));

  /* Class and device creation */
  if ((dev_class = class_create("gpio_drv")) ==
      NULL) // if ((dev_class = class_create(THIS_MODULE, "gpio_drv")) == NULL)
  {
    printk(KERN_ALERT "Class creation failed...\n");
    unregister_chrdev_region(first, 1);
    return -1;
  }

  if (device_create(dev_class, NULL, first, NULL, "gpio_drv%d", 0) == NULL) {
    printk(KERN_ALERT "Device creation failed...\n");
    class_destroy(dev_class);
    unregister_chrdev_region(first, 1);
    return -1;
  }
  /* File operations */
  cdev_init(&cdev, &fops);

  if (cdev_add(&cdev, first, 1) == -1) {
    printk(KERN_ALERT "Device addition failed...\n");
    device_destroy(dev_class, first);
    class_destroy(dev_class);
    unregister_chrdev_region(first, 1);
    return -1;
  }

  return 0;
}

static int sample_drv_remove(struct platform_device *pdev) {
  cdev_del(&cdev);
  device_destroy(dev_class, first);
  class_destroy(dev_class);
  unregister_chrdev_region(first, 1);
  printk(KERN_ALERT "Device unregistered...\n");

  return 0;
}

static const struct of_device_id gpio_led_dt[] = {
    {
        .compatible = "sample_gpio_led",
    },
    {}};

MODULE_DEVICE_TABLE(of, gpio_led_dt); /* dt stands for device tree */

static struct platform_driver sample_pldriver = {
    .driver =
        {
            .name = DRIVER_NAME,
            .of_match_table = of_match_ptr(gpio_led_dt),
        },
    .probe = sample_drv_probe,
    .remove = sample_drv_remove,
};

/* Initialization Module */
static int __init gpio_drv_init(void) {
  printk(KERN_ALERT "Welcome to the sample GPIO platform driver...\n");

  /* Register the platform driver */
  platform_driver_register(&sample_pldriver);
  return 0;
}

/* Exit Module */
static void __exit gpio_drv_exit(void) {
  printk(KERN_ALERT "Exiting the sample platform driver...Thanks...\n");

  /* Unregister the platform driver */
  platform_driver_unregister(&sample_pldriver);

  return;
}

module_init(gpio_drv_init);
module_exit(gpio_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("A. H.");
MODULE_DESCRIPTION("Platform GPIO device driver");
