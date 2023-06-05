#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "io.h"
#include "mm.h"
#include "session.h"

void fun(int *main_local_addr) {
  int fun_local;
  if (main_local_addr < &fun_local) {
    printf("STACK GROWS UPWARD\n");
  }
  else {
    printf("STACK GROWS DOWNWARD\n");
  }
}

void putc(void *p, char c) {
  if (c == '\n')
    uart_send('\r');
  uart_send(c);
}

void main(void){
  uart_init_gpio();
  init_printf(0, putc);

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  printf("\n\n\nMalinowyBASIC\n");
  printf("MalinowyBASIC\n");

  int rpiv = -1;

  #if RPI_VERSION == 3
  rpiv = 3;
  #elif RPI_VERSION == 4
  rpiv = 4;
  #endif

  printf("RPi version: %d\n", rpiv);

  int main_local;
  fun(&main_local);


  mem_init();
  print_memory_map();
  print_structures_size();
  printf("session init\n");
  Session *s = session_init();
  print_memory_map();

  session_end(s);
  printf("session end\n");
  print_memory_map();
}