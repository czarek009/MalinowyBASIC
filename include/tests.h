#include "printf.h"
#include "types.h"
#include "session.h"
#include "mm.h"
#include "gpio.h"
#include "utils.h"
#include "random.h"
#include "sd.h"
#include "hdmi.h"
#include "timer.h"
#include "images.h"


void test_hdmi() {
  printf("HDMI test\n");
  hdmi_draw_image(cat_320x200, 320, 200, 0, 0);
  delay_ms(1000);
}

void test_sd() {
  mbrS mbr;

  int r = sd_read(&mbr, sizeof(mbr));

  printf("Read disk returned: %d\n", r);

  if (mbr.bootSignature != BOOT_SIGNATURE) {
    printf("BAD BOOT SIGNATURE: %X\n", mbr.bootSignature);
  }

  u64 start = 0;

  for (int i=0; i<4; i++) {
    if (mbr.partitions[i].type == 0) {
      break;
    }

    printf("Partition %d:\n", i);
    printf("\t Type: %d\n", mbr.partitions[i].type);
    printf("\t NumSecs: %d\n", mbr.partitions[i].num_sectors);
    printf("\t Status: %d\n", mbr.partitions[i].status);
    printf("\t Start: 0x%X\n", mbr.partitions[i].first_lba_sector);
    start = mbr.partitions[i].first_lba_sector;
  }

  printf("[SD] Set position to %lu*512...\n", start);
  sd_seek(start*512);
  printf("Done.\n");

  char* rbuf1 = malloc(512);

  printf("[SD] Read data...\n");
  sd_read(rbuf1, 512);
  rbuf1[8] = '\0';
  printf("Data: %s\n", rbuf1);

  char* wbuf = malloc(512);
  wbuf = "qpad00pa";
  
  printf("[SD] Write data...\n");
  sd_write(wbuf, 512);
  printf("[SD] Done.\n");

  delay_ms(1000);
  char* rbuf2 = malloc(512);

  printf("[SD] Read data...\n");
  sd_read(rbuf2, 512);
  rbuf2[8] = '\0';
  printf("Data: %s\n", rbuf2);
}

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

void test_rng() {
  printf("Random number generator test\n");
  for (int i = 0; i < 100; ++i) {
    u64 rgn = rand(0, 100);
    printf("%lu ", rgn);
  }
  printf("\n");
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

void test_data_queue(sessionS *s) {
  printf("\nDATA STACK TEST\n");
  // push_data_to_queue(s, (s32)666);
  // push_data_to_queue(s, (s32)69);
  // push_data_to_queue(s, (s32)-42);
  print_data_queue(s);
  printf("pop = %d\n", read_data_from_queue(s));
  printf("pop = %d\n", read_data_from_queue(s));
  printf("pop = %d\n", read_data_from_queue(s));
  printf("next pop is on empty stack:\n");
  printf("pop = %d\n", read_data_from_queue(s));
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
  interpreter_process_input(s, "10 PRINT \"Fibonacci\"");
  interpreter_process_input(s, "20 INPUT \"Which element should I calculate? \" N");
  interpreter_process_input(s, "30 DIM TAB[N]");
  interpreter_process_input(s, "40 LET TAB[0] = 1");
  interpreter_process_input(s, "50 LET TAB[1] = 1");
  interpreter_process_input(s, "60 FOR I = 2 TO N-1 STEP 1");
  interpreter_process_input(s, "70 LET TAB[I] = TAB[I-1] + TAB[I-2]");
  interpreter_process_input(s, "80 NEXT");
  interpreter_process_input(s, "90 PRINT \"Fib(\";N;\") = \";TAB[N-1],");
  interpreter_process_input(s, "100 PRINT \"Fin\",");
}