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

void test_data_stack(Session *s) {
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

void test_return_sddress_stack(Session *s) {
  printf("\nRETURN ADDRESS STACK TEST\n");
  push_return_address_to_stack(s, (u64)2347878);
  push_return_address_to_stack(s, (u64)222);
  print_return_address_stack(s);
  printf("pop = %d\n", pop_return_address_from_stack(s));
  printf("pop = %d\n", pop_return_address_from_stack(s));
  printf("pop = %d\n", pop_return_address_from_stack(s));
}

void test_variable(Session *s) {
  printf("\nVARIABLE TEST\n");
  add_integer_variable(s, (s32)23, "int");
  add_floating_point_variable(s, (float)35.0, "float");
  add_string_variable(s, "data", "string");
  add_boolean_variable(s, true, "bool");
  print_variables(s);

  printf("adding variable with the same name (float)\n");
  add_integer_variable(s, (s32)69, "float");
  print_variables(s);
}

void test_instructions(Session *s) {
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

  mem_init();

  print_greetings();

  // struct_test();

  char buf[256];

  while (1) {
    readline(buf, "$> ");

    execute_command(NULL, buf);

  int main_local;
  fun(&main_local);

  mem_init();
  print_memory_map();
  print_structures_size();
  Session *s = session_init();
  print_memory_map();

  test_data_stack(s);
  test_return_sddress_stack(s);
  test_variable(s);
  test_instructions(s);

  printf("run program:\n");
  run_program(s);


  session_end(s);
  print_memory_map();
  //   uart_send_string("Recv: ");
  //   uart_send_string(buf);
  //   uart_send_string("\\0");
  //   uart_send_string("\r\n");
  }
}