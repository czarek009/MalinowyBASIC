#ifndef	_UART_H
#define	_UART_H

#include <inttypes.h>
#include <utils.h>
#include "peripherials.h"

#define TX 14
#define RX 15
#define ALTFN5 0b010

uint32_t uart_send_ready(void);
uint32_t uart_read_ready(void);
void uart_init(void);
char uart_recv(void);
void uart_send(char c);
void uart_send_string(char* str);

#endif  /*_UART_H */