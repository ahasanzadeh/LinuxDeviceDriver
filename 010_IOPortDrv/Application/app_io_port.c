#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <unistd.h>

/* Define macros for someports and IO port sections; we see this when we
 * use CAT /proc/ioport */
#define BASEPORT 0x70

int main() {
  char val = 0;

  /* Get access to the port */
  if (ioperm(BASEPORT, 4, 1)) /* Type man ioperm in a terminal to see the
                                 detail about it */
  {
    perror("ioperm"); /* Print a system error message */
    exit(1);
  }

  val = inb(BASEPORT + 1); /* Reading a byte */

  printf("Seconds = %x\n", val);

  /* We do not need the port anymore */
  if (ioperm(BASEPORT, 4, 0)) {
    printf("ioperm");
    exit(1);
  }

  return 0;
}
