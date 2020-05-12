# About the USB-Driver
It is a hello world USB driver program. Detailed working of the program is explained in the report. 

tested Linux-version: 5.3.0-51-generic

# steps to execute
1) run make command.
2) remove already present usb drivers - rmmod uas usb-storage
[above step to enforce our usb driver to manage the usb devices]
3) insert the module - insmod my_usb_driver.ko
4) run the tester - ./my_usb_tester