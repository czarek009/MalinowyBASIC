#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"


#define CLOCK_PIN 6
#define DATA_PIN 5


void keyboard_init(void);
char code_to_ascii(u16 code);

#endif /* _KEYBOARD_H */