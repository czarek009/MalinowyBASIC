#ifndef  _GPIO_H
#define  _GPIO_H

#include <inttypes.h>
#include <utils.h>
#include "peripherials.h"

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

void gpio_call(uint8_t pin, uint8_t value, uint8_t field_size, uint32_t regs[]);
void gpio_set(uint8_t pin, uint8_t set);
void gpio_clear(uint8_t pin, uint8_t set);
void gpio_func_selection(uint8_t pin, gpio_func func);
void gpio_pull(uint8_t pin, resistor res);

#endif  /*_GPIO_H */