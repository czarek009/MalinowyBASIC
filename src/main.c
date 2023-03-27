#include "uart.h"


void main(void){
  uart_init();
  uart_send_string("Hello, world!\r\n");

  #if RPI_VERSION == 3
  uart_send_string("RPi version: 3\r\n");

  #elif RPI_VERSION == 4
  uart_send_string("RPi version: 4\r\n");

  #endif

  while (1) {
    uart_send(uart_recv());
  }
}