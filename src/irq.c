#include "peripherials.h"
#include "utils.h"
#include "irq.h"
#include "printf.h"
#include "uart.h"

void enable_interrupt_controller(void) {
  put32(IRQ0_ENABLE_1, AUX_IRQ);
}

void handle_irq() {
  unsigned int irq = get32(IRQ0_PENDING_1);

  while (irq) {
    if (irq & AUX_IRQ) {
      irq &= ~AUX_IRQ;

      while ((get32(AUX_MU_IIR_REG) & 4) == 4) {
        printf("UART INTERRUPT. Recv: %c\n", uart_recv());
      }
    }
  }
}

void handle_invalid_irq(unsigned long esr_el1) {
  printf("[ERROR] Caught invalid interrupt. Value of est_el1 register: %X\n", esr_el1);
}