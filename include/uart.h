#ifndef  _UART_H
#define  _UART_H

#include "peripherials.h"
#include "gpio.h"
#include "types.h"

#define TX 14
#define RX 15
#define ALTFN5 0b010

u32 uart_send_ready(void);
u32 uart_read_ready(void);
void uart_init(void);
char uart_recv(void);
void uart_send(char c);
void uart_send_string(const char* str);
void uart_init_gpio(void);
void uart_aux(void);

#endif  /*_UART_H */