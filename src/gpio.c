#include "gpio.h"

void gpio_call(u8 pin, u8 value, u8 field_size, reg32 regs[]){
    if (pin >= NUM_OF_GPIO) return;
    u8 nr_of_fields = 32 / field_size;
    u8 start_bit = (pin % nr_of_fields) * field_size;
    u8 reg = pin / nr_of_fields;
    reg32 reg_val = regs[reg];
    u32 clearnig_mask = (1 << field_size) - 1;
    reg_val &= ~(clearnig_mask << start_bit);
    reg_val |= (value << start_bit);
    regs[reg] = reg_val;
}

void gpio_set(u8 pin, u8 set){
    gpio_call(pin, set, 1, GPIO_REGS->set_output.registers);
}
void gpio_clear(u8 pin, u8 set){
    gpio_call(pin, set, 1, GPIO_REGS->clear_output.registers);
}
void gpio_func_selection(u8 pin, gpio_func func){
    gpio_call(pin, func, 3, GPIO_REGS->func_select);
}
void gpio_pull(u8 pin, resistor res){
    #if RPI_VERSION == 3
    GPIO_REGS->pullup_pulldown_enable=res;
    gpio_call(pin, 1, 1, GPIO_REGS->pullup_pulldown_enable_clocks);

    #elif RPI_VERSION == 4
    gpio_call(pin, res, 2, GPIO_REGS->pullup_pulldown);
    #endif
}
