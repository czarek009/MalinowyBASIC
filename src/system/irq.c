#include "peripherials.h"
#include "utils.h"
#include "irq.h"
#include "uart.h"
#include "io.h"
#include "debug.h"
#include "gpio.h"
#include "keyboard.h"
#include "timer.h"
#include "hdmi.h"
#include "mm.h"


void enable_interrupt_controller(void) {
  // IRQ_REGS->irq0_enable_1 = AUX_IRQ | TIMER_CS_M1;
  IRQ_REGS->irq0_enable_1 = AUX_IRQ;// | TIMER_CS_M1;
  IRQ_REGS->irq0_enable_2 = GPIO0_IRQ;
}

void handle_irq() {
  irq_disable();
  #if RPI_VERSION == 3
  unsigned int irq1 = IRQ_REGS->irq0_pending_1;
  unsigned int irq2 = IRQ_REGS->irq0_pending_2;
  #elif RPI_VERSION == 4
  unsigned int irq1 = IRQ_REGS->irq0_pending_0;
  unsigned int irq2 = IRQ_REGS->irq0_pending_1;
  #endif

  while (irq1 || irq2) {

    if (irq1 & AUX_IRQ) {
      irq1 &= ~AUX_IRQ;

      while ((AUX_REGS->mu_iir & 4) == 4) {
        unsigned char c = uart_recv();
        io_read_char(c);
      }
    }

    // if (irq1 & TIMER_CS_M1) {
    //   irq1 &= ~TIMER_CS_M1;
    //   handle_timer_1();
    //   hdmi_blink_coursor();
    // }

    if (irq2 & GPIO0_IRQ) {
      uart_send('C');

      // static u8 counter = 0;
      // static u16 code = 0;

      // code |= gpio_level(DATA_PIN) << counter;
      // ++counter;

      // if (counter == 11) {
      //   // char c = code_to_ascii(code);
      //   // code = (code>>1) & 255;
      //   printf("%u\n", code);
      //   // printf("ascii: %u (%c)\n", (u16)c, c);
      //   counter = 0;
      //   code = 0;
      //   // io_read_char(c);
      // }
      irq2 &= ~GPIO0_IRQ;
      GPIO_REGS->event_detect_status.registers[0] = ~0;
    }
  }
  irq_enable();
}
__attribute__ ((__noinline__))
void * get_pc () { return __builtin_return_address(0); }
void handle_invalid_irq(unsigned long esr_el1) {
  printf("%lu\n", (u64)get_pc());
  ERROR("[ERROR] Caught invalid interrupt. Value of esr_el1 register: 0x%X\n", esr_el1);
}