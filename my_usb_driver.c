#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,11,12)
#include <asm/uaccess.h>
#else
#include <linux/uaccess.h>
#endif
/*usb_device to keep hold of the detected device structure */
static struct usb_device *detected_device;

/*usb_class_driver to create a device of usb type*/
static struct usb_class_driver my_usb_class;

/*List of usb devices that needs to be probed*/
static struct usb_device_id my_usbs[] = 
{
    {USB_DEVICE(0x0781,0x5567)}, /*SandDisk*/
    {USB_DEVICE(0x05ac,0x1392)}, /*Apple watch charger*/
    {USB_DEVICE(0x046d,0xc52f)}, /*Logitech Wireless Mouse*/
    {}
};

/*This function will export the expected usb_device_id's to be detected into the kernel*/
MODULE_DEVICE_TABLE(usb, my_usbs);

/*device open*/
int my_usb_open(struct inode *inode, struct file *file){
    return 0;
}

/*device release*/
int my_usb_release(struct inode *inode, struct file *file){
    return 0;
}

/*device write callback*/
ssize_t my_usb_write(struct file *file, const char *buf, size_t count, loff_t *ppos){
    char to_send_buffer[256];

    /*copying data from the user level and about to send to the USB device */
    copy_from_user(to_send_buffer, buf, sizeof(to_send_buffer));
    printk("About to send data: %s \n", to_send_buffer);

    /*Since the commercial USB are not flexible enough to establish a data trasfer
     this function is dummy, mentioning only the possible send functions */

    /*
    * usb_bulk_msg(...,usb_sndbulkpipe(..),...)
    * usb_interrupt_msg(...,usb_sndctrlpipe(..),...)
    * usb_control_msg(...,usb_sndintpipe(..),...)
    */
    return sizeof(to_send_buffer);
}

/*device read callback*/
ssize_t my_usb_read(struct file *file, char *buf, size_t count, loff_t *ppos){
    char buff[256];

    printk("About to receive data from USB device \n");

    /*Since the commercial USB are not flexible enough to establish a data trasfer
    this function is dummy, mentioning only the possible receive functions */
   /*
    * usb_bulk_msg(...,usb_rcvbulkpipe(..),...)
    * usb_interrupt_msg(...,usb_rcvctrlpipe(..),...)
    * usb_control_msg(...,usb_rcvintpipe(..),...)
    */

    /*sending a dummy message to the userspace*/
    sprintf(buff, "%s","dummy message");
    printk("Data Received:%s \n", buff);
    copy_to_user(buf, &buff, sizeof(buff));

    return sizeof(buf);
}

/*device ioctl callback*/
static long my_usb_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    /*Nothing to do. It can be implemented to perform multiple IOCTL operation from the userspace*/
    return 0;
}

/*defined the list of file-operations for the usb device*/
static struct file_operations my_usb_fops = {
    .owner = THIS_MODULE,
    .open = my_usb_open,
    .release = my_usb_release,
    .read = my_usb_read,
    .write= my_usb_write,
    .unlocked_ioctl = my_usb_ioctl
};

/*driver probe callback to be called when a device is detected */
int my_first_usb_probe(struct usb_interface *intf, const struct usb_device_id *id){
    
    /*interface_to_usbdev macro is an implementation of container_of function to extract the device from the probed interface*/
    detected_device = interface_to_usbdev(intf);

    printk("***********A new USB got detected*********** \n");
    printk("Following are the details of the device: \n");
    printk("Device Number: %d \n",detected_device->devnum);

    /*
    *   The interface number can be identifed from the usb_host_interface.
    *   usb_host_interface is embedded inside the usb_interface from where we can access the usb_host_interface's
    *   descriptor, which will give information about the interface.
    */
    printk("Detected Interface: %d\n", intf->cur_altsetting->desc.bInterfaceNumber);

    /*Interface number can also be directly obtained from the usb_device_id as well*/
    printk("Detected Interface through usb_device_id structure: %d \n", id->bInterfaceNumber);

    /*Vendor id and the product id*/
    printk("Vendor id: %04X \n", id->idVendor);
    printk("Product id: %04X\n", id->idProduct);

    /*Information about the device can be obtained from the device structure*/
    printk("Product Name: %s \n",detected_device->product );
    printk("Manufacturer Name: %s \n", detected_device->manufacturer);
    printk("Serial Number: %s \n",detected_device->serial);

    my_usb_class.name = "myfirstpen";
    my_usb_class.fops = &my_usb_fops;

    if(usb_register_dev(intf, &my_usb_class) < 0){
        printk("Unable to register and obtain a minor number for the device. \n");
        return -EINVAL;
    }
    printk("Registerd the device successfully. minor_number: %d", intf->minor);
    return 0;
}

/*driver disconnect callback to be called when the device is detached*/
void my_first_usb_disconnect(struct usb_interface *intf){
    /*deregistering the device on device disconnection*/
    usb_deregister_dev(intf, &my_usb_class);
    printk("my_first_usb_driver: The USB device with the device number: %d got disconnected \n", detected_device->devnum);
}

/*driver's ioctl callback*/
int my_first_usb_ioctl (struct usb_interface *intf, unsigned int code, void *buf){
    /*Userspace can trigger the ioctl through sysfs*/
    return 0;
}

/*the structure of the usb_driver that needs to be registered*/
static struct usb_driver my_first_usb_driver = {
    .name = "my_first_usb_driver",
    .probe = my_first_usb_probe,
    .disconnect = my_first_usb_disconnect,
    .unlocked_ioctl = my_first_usb_ioctl,
    .id_table = my_usbs,
};

/*Module entry function*/
static int __init my_first_usb_driver_init(void){
    if(usb_register(&my_first_usb_driver)){
        printk("Failed to register USB driver \n");
        return -EINVAL;
    }else{
        printk("my_first_usb_driver: USB driver registeration successful \n");
    }
    return 0;
}

/*Module exit function*/
static void __exit my_first_usb_driver_exit(void){
    usb_deregister(&my_first_usb_driver);
    printk("my_first_usb_driver: USB driver unregistered successfully \n");
}

module_init(my_first_usb_driver_init);
module_exit(my_first_usb_driver_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Keerthivasan");