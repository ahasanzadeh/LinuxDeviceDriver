/*
      Linux USB Device Driver
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
#include <linux/usb.h>

#define USB_VENDOR_ID 0xaaaa
#define USB_PRODUCT_ID 0x8816

/* FOPS also can be added in this code to open, read, write, release etc
 * for data transfer; hoever,in this code, we just are going to probe USB
 * driver */

struct usb_class_driver usb_cd;

static int usb_drv_probe(struct usb_interface *interface,
                         const struct usb_device_id *id) {
  struct usb_host_interface *interface_desc;
  struct usb_endpoint_descriptor *endpoint;
  int ret;

  interface_desc = interface->cur_altsetting;

  printk(KERN_INFO "USB info %d now probed: %04X , %04X\n",
         interface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct);
  printk(KERN_INFO "ID->bNumEndpoints: %02X\n",
         interface_desc->desc.bNumEndpoints);
  printk(KERN_INFO "ID->bInterfaceClass: %02X\n",
         interface_desc->desc.bInterfaceClass);

  usb_cd.name = "usb%d";
  ret = usb_register_dev(interface, &usb_cd);
  if (ret) {
    printk(KERN_INFO "Unable to get the minor number...\n");
    return ret;
  } else {
    printk(KERN_INFO "Minor number = %d\n", interface->minor);
  }

  return 0;
}

static void usb_drv_disconnect(struct usb_interface *interface) {
  printk(KERN_INFO "Disconnect and release the minor number %d\n",
         interface->minor);
  usb_deregister_dev(interface, &usb_cd);
}

static struct usb_device_id usb_drv_table[] = {
    {USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)}, {}};

MODULE_DEVICE_TABLE(usb, usb_drv_table);

static struct usb_driver usb_drv_struct = {
    .name = "A. H. USB Driver",
    .id_table = usb_drv_table,
    .probe = usb_drv_probe,
    .disconnect = usb_drv_disconnect,
};

/* Initialization Module */
static int __init usb_test_init(void) {
  printk(KERN_INFO "Register the USB driver with the USB subsystem...\n");
  usb_register(&usb_drv_struct);

  return 0;
}

/* Exit Module */
static void __exit usb_test_exit(void) {
  printk(KERN_INFO "Deregister the USB driver with the USB subsystem...\n");
  usb_deregister(&usb_drv_struct);
}

module_init(usb_test_init);
module_exit(usb_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("A. H.");
MODULE_DESCRIPTION("USB device driver");
