#include "peripherials.h"
#include "utils.h"
#include "uart.h"
#include "startup.h"


u32 uart_send_ready(void){
  return (AUX_REGS->mu_lsr & 0x20);
}

u32 uart_read_ready(void){
  return (AUX_REGS->mu_lsr & 0x01);
}

void uart_send(char c){
  while (!uart_send_ready());
  AUX_REGS->mu_io = c;
}

char uart_recv(void){
  while (!uart_read_ready());
  return (AUX_REGS->mu_io & 0xFF);
}

void uart_send_string(const char *str){
  for (int i = 0; str[i] != '\0'; i++) {
    uart_send((char)str[i]);
  }
}

void uart_aux(void){
  // enable uart1
  AUX_REGS->enables = 1;
  // disable TX and RX and auto flow control
  AUX_REGS->mu_control = 0;
  // disable receive and transmit interrupts
  // AUX_REGS->mu_ier = 2;
  AUX_REGS->mu_ier = 0xD;
  // enable 8 bit mode
  AUX_REGS->mu_lcr = 3;
  // RTS line to be always high
  AUX_REGS->mu_mcr = 0;

  #if RPI_VERSION == 3
  // 115200 @ 250 MHz (RPi 3)
  AUX_REGS->mu_baud_rate = 270;
  #elif RPI_VERSION == 4
  // 115200 @ 500 MHz (RPi 4)
  AUX_REGS->mu_baud_rate = 541;
  #endif

  // enable tx rx
  AUX_REGS->mu_control = 3;
}

void uart_init(void){
  u32 selector = GPIO_REGS->func_select[1];
  // tx use alt5
  selector &= ~(7 << 12);
  selector |= ALTFN5 << 12;
  // rx use alt5
  selector &= ~(7 << 15);
  selector |= ALTFN5 << 15;
  GPIO_REGS->func_select[1] = selector;

  #if RPI_VERSION == 3
  put32(PBASE + 0x00200094, 0);
  delay(150);
  put32(PBASE + 0x00200098, (1<<14) | (1<<15));
  delay(150);
  put32(PBASE + 0x00200098, 0);
  #elif RPI_VERSION == 4
  // clear pud tx
  u32 pu_pd = GPIO_REGS->pullup_pulldown[0];
  pu_pd &= ~(3 << 28);
  // clear pud rx
  pu_pd &= ~(3 << 30);
  GPIO_REGS->pullup_pulldown[0] = pu_pd;
  #endif

  uart_aux();

  delay(150);

  return;
}

void uart_init_gpio(void){
  gpio_func_selection(TX, ALT5);
  gpio_func_selection(RX, ALT5);
  gpio_pull(TX, NO_RESISTOR);
  gpio_pull(RX, NO_RESISTOR);
  uart_aux();
  delay(150);
}

void uart_startup_info(void) {
  STARTUP("[STARTUP]      Uart initialized\n");
  STARTUP("[UART]         TX pin: %d, RX pin: %d\n", TX, RX);
}