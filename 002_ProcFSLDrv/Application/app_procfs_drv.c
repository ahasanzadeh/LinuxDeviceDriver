#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int8_t write_buf[1024];
int8_t read_buf[1024];
int main()
{
    int fd; /* file descriptor */
    char option;

    printf("Welcome to the demo of character device driver...\n");

    fd = open("/proc/chr_proc", O_RDWR); /* Every character device driver
                                            is present in our device file
                                            in which we have written while
                                            creating the device which is
                                            my_device folder. Also, O_RDWR
                                            is read and write permission*/   
    if (fd < 0) 
    {
        printf("Cannot open device file...\n");
        return 0; /*why return 0 if we cannot open device file? */
    }

    while(1) /* Driver is alive, so an infinite loop is needed to
                read/write */
    {
        printf("*****Please enter your option*****\n");
        printf("            1. Write              \n");
        printf("            2. Read               \n");
        printf("            3. Exit               \n");
        scanf(" %c", &option);
        printf("  Your option is %c\n", option);

        switch(option)
        {
            case '1':
                printf("Enter the string to write into the driver: \n");
                scanf(" %[^\t\n]s", write_buf); /* %[^\t\n]s is format 
                                                   specifier when we
                                                   write the buffer */
                printf("Data written...");
                write(fd, write_buf, strlen(write_buf)+1);
                printf("Done...\n");
                break;
            case '2':
                printf("Data is reading...");
                read(fd, read_buf, 1024);
                printf("Done...\n\n");
                printf("Data is %s\n\n", read_buf);
                break;
            case '3':
                close(fd);
                exit(1); /* exit(1) and eit(EXIT_FAILURE) mean that the
                            program terminated abruptly with an error */
                break;
            default:
                printf("Enter a valid option = %c\n", option);
                break;
        }
    }
    close(fd);
}
