#include "uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "io.h"
#include "mm.h"
#include "session.h"
#include "interpreter.h"
#include "random.h"
#include "keyboard.h"
#include "timer.h"
#include "hdmi.h"
#include "sd.h"
#include "fs.h"


void print_greetings(void) {
  printf("\n\n");
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
  hdmi_printf_char(c);
}


void main(void) {
  uart_init_gpio();
  init_printf(0, putc);
  printf("\n\n");

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  mem_init();
  delay_ms(10);
  hdmi_init();
  delay_ms(10);
  timer_init();
  delay_ms(10);
  rand_init();
  sd_init();
  delay_ms(10);
  fs_init();
  delay_ms(10);
  keyboard_init();

  print_greetings();

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
  }
  /* hdmi allocates buffer - lickitung will show memory leak untill we call hdmi_end() */
  hdmi_end();
  lickitung_check();
}