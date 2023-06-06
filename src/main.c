#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "io.h"
#include "mm.h"
#include "interpreter.h"

void print_greetings(void) {
  printf("\n\n\nMalinowyBASIC\n");
  printf("MalinowyBASIC\n");

  int rpiv = -1;

  #if RPI_VERSION == 3
  rpiv = 3;
  #elif RPI_VERSION == 4
  rpiv = 4;
  #endif

  printf("RPi version: %d\n\n", rpiv);

}

void putc(void *p, char c) {
  if (c == '\n')
    uart_send('\r');
  uart_send(c);
}

void struct_test(void) {
  struct foo {
    int id;
    char name[8];
  } typedef fooS;

  fooS bar[] = {
    {69, "qpa"},
    {42, "d00pa"},
  };

  for (int i=0; i<2; ++i) {
    printf(bar[i].name);
  }
}

void main(void){
  uart_init_gpio();
  init_printf(0, putc);

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  mem_init();

  print_greetings();

  // struct_test();

  char buf[256];

  while (1) {
    readline(buf, "$> ");

    execute_command(NULL, buf);

  //   uart_send_string("Recv: ");
  //   uart_send_string(buf);
  //   uart_send_string("\\0");
  //   uart_send_string("\r\n");
  }
}