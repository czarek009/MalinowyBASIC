#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "io.h"
#include "mm.h"
#include "session.h"
#include "interpreter.h"
#include "tests.h"


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
  uart_init_gpio();
  init_printf(0, putc);

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  mem_init();

  print_greetings();

  sessionS *current_session = session_init();

  add_instruction(current_session, 10, "FOR X = 0 TO 9 STEP 1");
  add_instruction(current_session, 20, "FOR Y = 0 TO 9 STEP 1");
  add_instruction(current_session, 30, "PRINT X; Y; \" \"");
  add_instruction(current_session, 40, "NEXT");
  add_instruction(current_session, 50, "NEXT");
  add_instruction(current_session, 60, "PRINT \"End\",");

  while (1) {
    printf("START SESSION\n");
    sessionS *current_session = session_init();

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