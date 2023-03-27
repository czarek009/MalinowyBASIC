#include "uart.h"


uint32_t uart_send_ready(void){
  return (get32(AUX_MU_LSR_REG) & 0x20);
}

uint32_t uart_read_ready(void){
  return (get32(AUX_MU_LSR_REG) & 0x01);
}

void uart_send(char c){
  while (!uart_send_ready());
  put32(AUX_MU_IO_REG, c);
}

char uart_recv(void){
  while (!uart_read_ready());
  return (get32(AUX_MU_IO_REG) & 0xFF);
}

void uart_send_string(char *str){
  for (int i = 0; str[i] != '\0'; i++) {
    uart_send((char)str[i]);
  }
}

void uart_init(void){

  uint32_t selector = get32(GPFSEL1);
  // tx use alt5
  selector &= ~(7 << 12);
  selector |= ALTFN5 << 12;
  // rx use alt5
  selector &= ~(7 << 15);
  selector |= ALTFN5 << 15;
  put32(GPFSEL1, selector);

  #if RPI_VERSION == 3
  put32(PBASE + 0x00200094, 0);
  delay(150);
  put32(PBASE + 0x00200098, (1<<14) | (1<<15));
  delay(150);
  put32(PBASE + 0x00200098, 0);
  #elif RPI_VERSION == 4
  // clear pud tx
  uint32_t pu_pd = get32(GPIO_PUP_PDN_CNTRL_REG0);
  pu_pd &= ~(3 << 28);
  // clear pud rx
  pu_pd &= ~(3 << 30);
  put32(GPIO_PUP_PDN_CNTRL_REG0, pu_pd);
  #endif

  // enable uart1
  put32(AUX_ENABLES, 1);
  // disable TX and RX and auto flow control
  put32(AUX_MU_CNTL_REG, 0);
  // disable receive and transmit interrupts
  put32(AUX_MU_IER_REG, 0);
  // enable 8 bit mode
  put32(AUX_MU_LCR_REG, 3);
  // RTS line to be always high
  put32(AUX_MU_MCR_REG, 0);

  #if RPI_VERSION == 3
  // 115200 @ 250 MHz (RPi 3)
  put32(AUX_MU_BAUD_REG, 270);
  #elif RPI_VERSION == 4
  // 115200 @ 500 MHz (RPi 4)
  put32(AUX_MU_BAUD_REG, 541);
  #endif

  // enable tx rx
  put32(AUX_MU_CNTL_REG, 3);

  delay(150);

  return;
}