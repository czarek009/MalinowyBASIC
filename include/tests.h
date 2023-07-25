#include "printf.h"
#include "types.h"
#include "session.h"
#include "mm.h"
#include "gpio.h"
#include "utils.h"


void test_fpu() {
  float x = 17.24;
  float y = 25.33;
  printf("FPU: %u\n", (u32)(x+y));
}

void test_gpio() {
  gpio_func_selection(16, OUTPUT);
  for (int i = 0; i < 4; ++i) {
    gpio_set(16);
    delay(1000);
    gpio_clear(16);
    delay(1000);
  }
}

void test_malloc() {
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

void test_arrays(sessionS *s) {
  printf("\nARRAY TEST - FIB\n");
  printf("to see added instructions - write LIST\n");
  printf("to run program - write RUN\n\n");
  add_instruction(s, 10, "PRINT \"Fibonacci\",");
  add_instruction(s, 20, "INPUT \"Which element should I calculate? \" N");
  add_instruction(s, 30, "DIM TAB[N]");
  add_instruction(s, 40, "LET TAB[0] = 1");
  add_instruction(s, 50, "LET TAB[1] = 1");
  add_instruction(s, 60, "FOR I = 2 TO N-1 STEP 1");
  add_instruction(s, 70, "LET TAB[I] = TAB[I-1] + TAB[I-2]");
  add_instruction(s, 80, "NEXT");
  add_instruction(s, 90, "PRINT \"Fib(\";N;\") = \";TAB[N-1],");
  add_instruction(s, 100, "PRINT \"Fin\",");
}