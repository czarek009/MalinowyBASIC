#include "io.h"
#include "irq.h"
#include "uart.h"
#include "utils.h"
#include "hdmi.h"

volatile static char line_buffer[255];
volatile static bool refresh;
volatile static bool eol;
volatile static u8 idx;

void io_reset(void) {
  line_buffer[0] = '\0';
  refresh = false;
  eol = false;
  idx = 0;
}

void print_line(const char* prompt) {
  uart_send('\r');
  uart_send_string(prompt);
  for (int i = 0; i < idx; ++i) {
    uart_send(line_buffer[i]);
  }
}

void clear_line(void) {
  uart_send('\r');
  uart_send_string("   "); // should be prompt length
  for (int i = 0; i < idx; ++i) {
    uart_send(' ');
  }
  uart_send(' ');
}

void io_read_char(char c) {
  if (c == 13) {
    // enter
    eol = true;
  } else if (c == 96) {
    // alternative enter `
    // because enter doesn't always work on my keyboard :(
    eol = true;
  } else if (c > 31 && c < 127) {
    // regular character
    line_buffer[idx] = c;
    idx++;
  } else if (c == 127 || c == 8) {
    // backspace
    if (idx > 0) {
      idx--;
      line_buffer[idx] = ' ';
    }
  }
  hdmi_printf_char(c);
  refresh = true;
}

void readline(char* restrict dest, const char* prompt) {
  io_reset();
  uart_send('\r');
  uart_send_string(prompt);
  hdmi_printf_string(prompt);
  while (!eol) {
    while(!refresh);
    clear_line();
    print_line(prompt);
    refresh = false;
  }

  irq_disable();
  for (int i = 0; i < idx; ++i) {
    dest[i] = line_buffer[i];
  }
  dest[idx] = '\0';
  irq_enable();

  uart_send('\r');
  uart_send('\n');
  hdmi_printf_char('\n');
}