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
#include "timer.h"
#include "hdmi.h"
#include "sd.h"
#include "fs.h"


static bool ready = false;


void print_greetings(void) {
  printf("\n\nMalinowyBASIC\n");

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

  mem_init();

  hdmi_init();
  rand_init();
  // timer_init();
  init_keyboard();
  sd_init();
  fs_init();

  irq_init_vectors();
  enable_interrupt_controller();
  irq_enable();

  ready = true;

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

void hdmi_cpu(void) {
  u64 counter = 0;
  while(!ready) {
    delay_ms(10);
  }
  while(1) {
    if(counter % 12 == 0){
      hdmi_blink_coursor();
      counter = 0;
    }
    hdmi_refresh();
    delay_ms(40);  // 25 FPS
    counter++;
  }
}