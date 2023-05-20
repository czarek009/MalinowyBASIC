#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "mm.h"

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
  void *p1 = mmalloc(5);
  printf("pointer = %lu\n", (u64)p1);
  void *p2 = mmalloc(1023);
  printf("pointer = %lu\n", (u64)p2);
  void *p3 = mmalloc(69);
  printf("pointer = %lu\n", (u64)p3);



  while (1) {
  }
}