#include "gpio.h"

void gpio_call(uint8_t pin, uint8_t value, uint8_t field_size, uint32_t regs[]){
    if (pin >= NUM_OF_GPIO) return;
    uint8_t nr_of_fields = 32 / field_size;
    uint8_t start_bit = (pin % nr_of_fields) * field_size;
    uint8_t reg = pin / nr_of_fields;
    uint32_t reg_addr = regs[reg];
    uint32_t reg_val = get32(reg_addr);
    uint32_t clearnig_mask = (1 << field_size) - 1;
    reg_val &= ~(clearnig_mask << start_bit);
    reg_val |= (value << start_bit);
    put32(reg_addr, reg_val);
}

void gpio_set(uint8_t pin, uint8_t set){
    gpio_call(pin, set, 1, (uint32_t[]){GPSET0, GPSET1});
}
void gpio_clear(uint8_t pin, uint8_t set){
    gpio_call(pin, set, 1, (uint32_t[]){GPCLR0, GPCLR1});
}
void gpio_func_selection(uint8_t pin, gpio_func func){
    gpio_call(pin, func, 3, (uint32_t[]){GPFSEL0, GPFSEL1, GPFSEL2, GPFSEL3, GPFSEL4, GPFSEL5});
}
void gpio_pull(uint8_t pin, resistor res){
    #if RPI_VERSION == 3
    put32(GPPUD, res);
    gpio_call(pin, 1, 1, (uint32_t[]){GPPUDCLK0, GPPUDCLK0});

    #elif RPI_VERSION == 4
    gpio_call(pin, res, 2, (uint32_t[]){GPIO_PUP_PDN_CNTRL_REG0, GPIO_PUP_PDN_CNTRL_REG1, GPIO_PUP_PDN_CNTRL_REG2, GPIO_PUP_PDN_CNTRL_REG3});
    #endif
}
