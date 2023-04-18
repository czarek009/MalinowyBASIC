#ifndef  _GPIO_H
#define  _GPIO_H

#include "peripherials.h"
#include "types.h"

typedef enum {
    INPUT = 0b000,
    OUTPUT = 0b001,
    ALT0 = 0b100,
    ALT1 = 0b101,
    ALT2 = 0b110,
    ALT3 = 0b111,
    ALT4 = 0b011,
    ALT5 = 0b010
} gpio_func;

typedef enum {
    NO_RESISTOR = 0b00,
    #if RPI_VERSION == 3
    PULL_DOWN = 0b01,
    PULL_UP = 0b10

    #elif RPI_VERSION == 4
    PULL_UP = 0b01,
    PULL_DOWN = 0b10
    #endif
} resistor;

void gpio_call(u8 pin, u8 value, u8 field_size, reg32 regs[]);
void gpio_set(u8 pin, u8 set);
void gpio_clear(u8 pin, u8 set);
void gpio_func_selection(u8 pin, gpio_func func);
void gpio_pull(u8 pin, resistor res);

#endif  /*_GPIO_H */