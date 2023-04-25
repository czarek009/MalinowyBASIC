#include "uart.h"
#include "printf.h"


void putc(void *p, char c) {
  if (c == '\n')
    uart_send('\r');
  uart_send(c);
}

void main(void){
  // uart_init();
  uart_init_gpio();
  init_printf(0, putc);

  printf("Hello, world!\n");

  int rpiv = 0;

  #if RPI_VERSION == 3
    rpiv = 3;
  #elif RPI_VERSION == 4
    rpiv = 4;
  #endif

  printf("RPi version: %d\n", rpiv);

  printf("\nchecking GPIO module:\n");
  gpio_func_selection(17, OUTPUT);
  printf("gpio level = %u\n", gpio_level(17));
  gpio_set(17);
  printf("GPIO SET\n");
  printf("gpio level = %u\n", gpio_level(17));
  gpio_clear(17);
  printf("GPIO CLEAR\n");
  printf("gpio level = %u\n", gpio_level(17));

  printf("\ncheck echo:\n");
  while (1) {
    uart_send(uart_recv());
  }
}