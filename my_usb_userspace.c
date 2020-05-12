#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    int fd;
    char buf[256] = "Hello USB World!";
    char retval[256];
    fd = open("/dev/myfirstpen",O_RDWR);
    if(fd < 0){
        printf("Failed to open the file \n");
        return -1;
    }

    printf("About to send: %s \n", buf);
    /*Writing to the USB device */
    if(write(fd, &buf, sizeof(buf)) < 0){
        printf("Failed to write to the device \n");
        return -1;
    }
    /*Reading back from the USB device */
    read(fd, &retval, sizeof(retval));
    printf("Read the following data: %s \n", retval);

    close(fd);
    printf("User program exiting!! \n");
    return 0;
}