#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "io.h"
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
  delay(150);
  void *p1 = malloc(5);
  printf("pointer = %lu\n", (u64)p1);
  print_memory_map();
  void *p2 = malloc(1023);
  printf("pointer = %lu\n", (u64)p2);
  print_memory_map();
  void *p3 = malloc(69);
  printf("pointer = %lu\n", (u64)p3);
  void *p4 = malloc(13);
  printf("pointer = %lu\n", (u64)p4);
  print_memory_map();
  free(p2);
  print_memory_map();
  free(p3);
  print_memory_map();
  free(p1);
  print_memory_map();
  free(p4);
  print_memory_map();

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