/*
      I2C client Driver
      Written by A. H.
*/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h> //For kernel module programming like module_init
                          //and module_exit etc
#include <asm/io.h>
#include <linux/cdev.h> //For cdev structure
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h> //For file structure
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kdev_t.h> //For device files
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/log2.h>
#include <linux/mod_devicetable.h>
#include <linux/mutex.h>
#include <linux/slab.h> //For kmalloc()
#include <linux/sysfs.h>
#include <linux/timer.h>
#include <linux/uaccess.h> //For function calls like copy_to/from_user()

struct i2c_data {
  struct i2c_client *client;
  dev_t dev; /* For major and minor number */
  u8 *buf;
  u16 value;
  struct cdev cdev;
  struct class *class;
};

/******************** Driver Functions ********************/
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char __user *buf, size_t len,
                       loff_t *off);
static ssize_t my_write(struct file *filp, const char *buf, size_t len,
                        loff_t *off);

/******************** File Operation Structure ********************/
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
    .open = my_open,
    .release = my_release,
};

/******************** Function Definition ********************/
static int my_open(struct inode *inode, struct file *file) {
  struct i2c_data *dev = container_of(inode->i_cdev, struct i2c_data, cdev);
  if (dev == NULL) {
    printk(KERN_INFO "There is no data...\n");
    return -1;
  }
  file->private_data = dev;

  return 0;
}

static int my_release(struct inode *inode, struct file *file) {
  printk(KERN_INFO "Device file closed...\n");
  return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len,
                       loff_t *off) {
  struct i2c_data *dev = (struct i2c_data *)(filp->private_data);
  struct i2c_adapter *adap = dev->client->adapter;
  struct i2c_msg msg;
  char *temp;
  int ret;

  temp = kmalloc(len, GFP_KERNEL);

  msg.addr = 0x68;
  msg.flags = 0;
  msg.flags |= I2C_M_RD;
  msg.len = len;
  msg.buf = temp;

  ret = i2c_transfer(adap, &msg, 1);
  if (ret >= 0) {
    ret = copy_to_user(buf, temp, len) ? -EFAULT : len;
  }
  kfree(temp);
  return ret;
}

static ssize_t my_write(
    struct file *filp, const char __user *buf, size_t len,
    loff_t *off) /*should __user be here? can I use count instead of len here?*/
{
  struct i2c_data *dev = (struct i2c_data *)(filp->private_data);
  struct i2c_adapter *adap = dev->client->adapter;
  struct i2c_msg msg;
  char *temp;
  int ret;

  temp = memdup_user(buf, len);

  msg.addr = 0x68;
  msg.flags = 0;
  msg.len = len;
  msg.buf = temp;

  ret = i2c_transfer(adap, &msg, 1);
  kfree(temp);
  return (ret == 1 ? len : ret);
}

/* Remove function */
static void
ds3231_remove(struct i2c_client *client) /* According to i2c_driver struct,
                                            remove should be void and not int */
{
  struct i2c_data *data;
  printk(KERN_INFO "Remove function is inviked...\n");
  data = i2c_get_clientdata(client);
  cdev_del(&data->cdev);
  device_destroy(data->class, data->dev);
  class_destroy(data->class);
  unregister_chrdev_region(data->dev, 1);
  // return 0;
}

/* Probe function to invoke the I2C driver */
static int
ds3231_probe(struct i2c_client *
                 client) /* According to i2c_driver struct,
                             probe should have one argument, and 2nd argument
                            "const struct i2c_device_id *id" should be removed*/
{
  struct i2c_data *data;
  int result;

  printk(KERN_INFO "Probe function is invoked...\n");
  data = devm_kzalloc(&client->dev, sizeof(struct i2c_data), GFP_KERNEL);
  data->value = 30;
  data->buf = devm_kzalloc(&client->dev, data->value, GFP_KERNEL);
  i2c_set_clientdata(client, data);

  result = alloc_chrdev_region(&data->dev, 0, 1, "i2c_drv");
  if (result < 0) {
    printk(KERN_INFO "Unable to do device registration...\n");
    unregister_chrdev_region(data->dev, 1);
    return -1;
  }

  printk(KERN_INFO "Major Number = %d\n", MAJOR(data->dev));
  if ((data->class = class_create("i2cdriver")) ==
      NULL) // if ((data->class = class_create(THIS_MODULE, "i2cdriver")) ==
            // NULL)
  {
    printk(KERN_INFO "Unable to create the device class...\n");
    unregister_chrdev_region(data->dev, 1);
    return -1;
  }

  if (device_create(data->class, NULL, data->dev, NULL, "i2c_drv%d", 0) ==
      NULL) {
    printk(KERN_INFO "Unable to create the device...\n");
    class_destroy(data->class);
    unregister_chrdev_region(data->dev, 1);
    return -1;
  }

  cdev_init(&data->cdev, &fops);
  if (cdev_add(&data->cdev, data->dev, 1) == -1) {
    printk(KERN_INFO "Unable to add the device...\n");
    device_destroy(data->class, data->dev);
    class_destroy(data->class);
    unregister_chrdev_region(data->dev, 1);
    return -1;
  }

  return 0;
}

static const struct i2c_device_id i2c_ids[] = {{"ds3231", 0}, {"ds32", 0}, {}};

MODULE_DEVICE_TABLE(i2c, i2c_ids);

static struct i2c_driver ds3231_i2c_drv = {
    .driver =
        {
            .name = "ds32",
            .owner = THIS_MODULE,
        },
    .probe = ds3231_probe,
    .remove = ds3231_remove,
    .id_table = i2c_ids,
};

/* Initialization Module */
static int __init i2c_client_drv_init(void) {
  /* Register with I2C core */
  return i2c_add_driver(&ds3231_i2c_drv);
}

/* Exit Module */
static void __exit i2c_client_drv_exit(void) {
  i2c_del_driver(&ds3231_i2c_drv);
}

module_init(i2c_client_drv_init);
module_exit(i2c_client_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("A. H.");
MODULE_DESCRIPTION("I2C Client Driver");
