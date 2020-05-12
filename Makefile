CC=gcc

APP = my_usb_tester
obj-m += my_usb_driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o $(APP) my_usb_userspace.c -Wall

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f $(APP) 