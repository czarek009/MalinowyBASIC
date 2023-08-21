#include "startup.h"
#include "keyboard.h"
#include "timer.h"
#include "hdmi.h"
#include "sd.h"
#include "random.h"
#include "uart.h"
#include "io.h"
#include "mm.h"
#include "irq.h"
#include "fs.h"
#include "sd.h"
#include "images.h"

/* PRIVATE FUNCTIONS DECLARATIONS */
void putc(void *p, char c);
void print_greetings(void);
void print_modules_info(void);

/* GLOBAL FUNCTIONS DEFINITIONS */
void initialize_all_modules(void) {
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
  delay_ms(10);
  rand_init();
  sd_init();
  delay_ms(10);
  fs_init();
  delay_ms(10);
  keyboard_init();
}

void print_startup_info(void) {
  hdmi_clear();
  print_greetings();
  print_modules_info();
  delay_ms(5000);
  hdmi_clear();
  hdmi_draw_image(malinka, MALINKA_WIDTH, MALINKA_HEIGHT, 60, 50);
  hdmi_draw_image(napis, NAPIS_WIDTH, NAPIS_HEIGHT, 230, 90);
  hdmi_draw_image(autorzy, AUTORZY_WIDTH, AUTORZY_HEIGHT, 215, 315);
  hdmi_refresh();
  delay_ms(5000);
  hdmi_clear();
  timer_init();
}

/* PRIVATE FUNCTIONS DEFINITIONS */
void putc(void *p, char c) {
  if (c == '\n')
    uart_send('\r');
  uart_send(c);
  hdmi_printf_char(c);
}

void print_greetings(void) {
  STARTUP("         MalinowyBASIC\n");
  int rpiv = -1;
  #if RPI_VERSION == 3
  rpiv = 3;
  #elif RPI_VERSION == 4
  rpiv = 4;
  #endif
  STARTUP("         RPi version: %d\n\n", rpiv);
}

void print_modules_info(void) {
  uart_startup_info();
  irq_startup_info();
  mem_startup_info();
  hdmi_startup_info();
  timer_startup_info();
  rand_startup_info();
  sd_startup_info();
  fs_startup_info();
  keyboard_startup_info();
}