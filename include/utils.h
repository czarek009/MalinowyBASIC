#ifndef  _UTILS_H
#define  _UTILS_H

void delay(unsigned long);
void put32(unsigned long, unsigned int);
unsigned int get32(unsigned long);
unsigned int get_el(void);
void set_el1(void);

#endif  /*_UTILS_H */