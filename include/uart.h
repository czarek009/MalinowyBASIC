#ifndef  _UART_H
#define  _UART_H

#include "types.h"

#define TX 14
#define RX 15
#define ALTFN5 0b010

u32 uart_send_ready(void);
u32 uart_read_ready(void);
void uart_init(void);
char uart_recv(void);
void uart_send(char c);
void uart_send_string(char* str);

#endif  /*_UART_H */