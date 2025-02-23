#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define GPIO_SELECT_LED _IOW('1', 1, int)

int main() {
  int fd = -1; /* file descriptor */
  int option;
  int count = 1;
  unsigned char c;

  printf("Welcome to the demo of platform GPIO LED device driver...\n");

  fd = open("/dev/gpio_drv0", O_RDWR); /* See line 116 on driver side C
                                          file where there is "gpio_drv%d
                                          which %d is 0 as net argument
                                          in the same line 116. We also
                                          need to open O it in read Rd
                                          and write WR mode */
  if (fd == -1) {
    perror("led_operations open");
  }

  do /* Driver is alive, so an infinite loop is needed to
              read/write */
  {
    printf("1. Getting the LED status...\n");
    printf("2. Set the LED status...\n");
    printf("3. Close...\n");
    printf("0. Exit...\n");
    printf("Enter your option...\n");
    scanf(" %d", &option);
    printf("  Your option is %d\n", option);
    getchar();

    switch (option) {
    case 0:
      break;
    case 1:
      if (fd == -1) {
        perror("File not open...\n");
      }
      count = read(fd, &c, 1);
      if (count == -1) {
        perror("led_operations read");
      } else {
        printf("\n LED value is %c", c);
      }

      printf("\n");
      break;
    case 2:
      printf("Enter your option... [0 - Turn off the LED, 1 - Turn on the "
             "LED...\n");
      c = getchar();
      count = write(fd, &c, 1);
      if (c == -1) {
        perror("led_operations write");
        break;
      }
      break;
    case 3:
      close(fd);
      fd = -1;
      break;
    default:
      printf("Enter a valid option = %d\n", option);
      break;
    }
  } while (option != 0);

  return 0;
}
