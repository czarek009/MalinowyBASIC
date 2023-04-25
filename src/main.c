#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"


void putc(void *p, char c) {
  if (c == '\n')
    uart_send('\r');
  uart_send(c);
}

void main(void){
  uart_init();
  init_printf(0, putc);

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  printf("MalinowyBASIC\n");

  int rpiv = -1;
  int el = get_el();

  #if RPI_VERSION == 3
  rpiv = 3;
  #elif RPI_VERSION == 4
  rpiv = 4;
  #endif

  printf("RPi version: %d\n", rpiv);
  printf("Exception level: %d\n", el);

  printf("\n");
  while (1) {
  }
}