#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "io.h"
#include "mm.h"
#include "session.h"
#include "interpreter.h"
#include "tests.h"
#include "random.h"
#include "keyboard.h"



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


void main(void){
  rand_init();
  init_keyboard();

  uart_init_gpio();
  init_printf(0, putc);

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  mem_init();

  print_greetings();

  printf("Random number generator test\n");
  for (int i = 0; i < 100; ++i) {
    u64 rgn = rand(0, 100);
    printf("%lu ", rgn);
  }
  printf("\n");

  while (1) {
    printf("START SESSION\n");
    sessionS *current_session = session_init();
    test_arrays(current_session);

    while (1) {
      char buf[256] = {0};
      sessionErrorCodeE result = SESSION_NO_ERROR;
      readline(buf, "$> ");
      result = interpreter_process_input(current_session, buf);

      if (result == SESSION_END) {
        printf("SESSION END\n");
        break;
      }
      if (result != SESSION_NO_ERROR) {
        ERROR("ERROR\n");
      }
    }
    session_end(current_session);
    lickitung_check();
  }
}