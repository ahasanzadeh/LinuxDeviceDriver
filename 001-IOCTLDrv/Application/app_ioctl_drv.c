#include "header.h"

#define WR_DATA _IOW('a', 'a', int32_t*)
#define RD_DATA _IOR('a', 'b', int32_t*)

int main()
{
    int fd;
    int32_t val, num;
    printf("\n IOCTL based character device driver operation from user space...\n");
        
    fd = open("/dev/my_device", O_RDWR);
    if (fd < 0) 
    {
        printf("Cannot open the device file...\n");
        return 0;
    }
    
    printf("Enter the data to send...\n");
    scanf("%d", &num);
    printf("Writing value to the driver...\n");
    ioctl(fd, WR_DATA, (int32_t*)&num);

    printf("Reading value from the driver...\n");
    ioctl(fd, RD_DATA, (int32_t*)&val);
    printf("Value is %d\n", val);

    printf("Closing the driver...\n");
    close(fd);
}
