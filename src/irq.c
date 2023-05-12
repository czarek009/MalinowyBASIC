#include "peripherials.h"
#include "utils.h"
#include "irq.h"
#include "printf.h"
#include "uart.h"
#include "io.h"

void enable_interrupt_controller(void) {
  IRQ_REGS->irq0_enable_1 = AUX_IRQ;
}

void handle_irq() {
  unsigned int irq = IRQ_REGS->irq0_pending_1;

  while (irq) {
    if (irq & AUX_IRQ) {
      irq &= ~AUX_IRQ;

      while ((AUX_REGS->mu_iir & 4) == 4) {
        unsigned char c = uart_recv();
        io_read_char(c);
      }
    }
  }
}

void handle_invalid_irq(unsigned long esr_el1) {
  printf("[ERROR] Caught invalid interrupt. Value of est_el1 register: %X\n", esr_el1);
}