#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "io.h"
#include "mm.h"
#include "session.h"
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

void test_data_stack(sessionS *s) {
  printf("\nDATA STACK TEST\n");
  push_data_to_stack(s, (s32)666);
  push_data_to_stack(s, (s32)69);
  push_data_to_stack(s, (s32)-42);
  print_data_stack(s);
  printf("pop = %d\n", pop_data_from_stack(s));
  printf("pop = %d\n", pop_data_from_stack(s));
  printf("pop = %d\n", pop_data_from_stack(s));
  printf("next pop is on empty stack:\n");
  printf("pop = %d\n", pop_data_from_stack(s));
}

void test_return_sddress_stack(sessionS *s) {
  printf("\nRETURN ADDRESS STACK TEST\n");
  push_return_address_to_stack(s, (u64)2347878);
  push_return_address_to_stack(s, (u64)222);
  print_return_address_stack(s);
  printf("pop = %d\n", pop_return_address_from_stack(s));
  printf("pop = %d\n", pop_return_address_from_stack(s));
  printf("pop = %d\n", pop_return_address_from_stack(s));
}

void test_variable(sessionS *s) {
  printf("\nVARIABLE TEST\n");
  add_integer_variable(s, (s32)23, "int");
  add_floating_point_variable(s, (double)35.0, "float");
  add_integer_variable(s, (s64)23, "int");
  add_floating_point_variable(s, (float)35.0, "float");
  add_string_variable(s, "data", "string");
  add_boolean_variable(s, true, "bool");
  print_variables(s);

  printf("adding variable with the same name (float)\n");
  add_integer_variable(s, (s64)69, "float");
  print_variables(s);
}

void test_instructions(sessionS *s) {
  printf("\nINSTRUCTIONS TEST\n");
  add_instruction(s, (u64)5, "let y = x + 1");
  add_instruction(s, (u64)1, "let x = 1");
  add_instruction(s, (u64)8, "print sum");
  add_instruction(s, (u64)6, "let sum = x + y");

  print_instructions(s);
}

void main(void){
  uart_init_gpio();
  init_printf(0, putc);

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  print_greetings();

  mem_init();

  print_memory_map();
  delay(150);
  void *p1 = malloc(5);
  printf("pointer = %lu\n", (u64)p1);
  print_memory_map();
  void *p2 = malloc(1023);
  printf("pointer = %lu\n", (u64)p2);
  print_memory_map();
  void *p3 = malloc(9);
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

  float x = 17.24;
  float y = 25.33;
  printf("FPU: %u\n", (u32)(x+y));

  gpio_func_selection(16, OUTPUT);
  for (int i = 0; i < 4; ++i) {
    gpio_set(16);
    delay(1000);
    gpio_clear(16);
    delay(1000);
  }

  sessionS *current_session = session_init();

  add_floating_point_variable(current_session, 69.9999, "check");

  while (1) {
    char buf[256] = {0};
    readline(buf, "$> ");
    execute_command(current_session, buf);
  }
  session_end(current_session);
}