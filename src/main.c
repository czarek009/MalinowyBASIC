#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "io.h"
#include "mm.h"


void putc(void *p, char c) {
  if (c == '\n')
    uart_send('\r');
  uart_send(c);
}

void main(void){
  // uart_init();
  uart_init_gpio();
  init_printf(0, putc);

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  printf("\n\n\nMalinowyBASIC\n");

  int rpiv = -1;
  int el = get_el();

  #if RPI_VERSION == 3
  rpiv = 3;
  #elif RPI_VERSION == 4
  rpiv = 4;
  #endif

  printf("RPi version: %d\n", rpiv);
  printf("Exception level: %d\n", el);

  gpio_func_selection(16, OUTPUT);
  for (int i = 0; i < 4; ++i) {
    gpio_set(16);
    delay(1000);
    gpio_clear(16);
    delay(1000);
  }

  char buf[256];
  while (1) {
    readline(buf, "$> ");

    uart_send_string("Recv: ");
    uart_send_string(buf);
    uart_send_string("\\0");
    uart_send_string("\r\n");
  }
}