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


void enable_interrupt_controller(void) {
  IRQ_REGS->irq0_enable_1 = AUX_IRQ | TIMER_CS_M1;
  IRQ_REGS->irq0_enable_2 = GPIO0_IRQ;
}

void handle_irq() {
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

    if (irq1 & TIMER_CS_M1) {
      irq1 &= ~TIMER_CS_M1;
      handle_timer_1();
      hdmi_blink_coursor();
    }

    if (irq2 & GPIO0_IRQ) {
      u16 code = 0;
      for(int i=0; i<11; i++) {
        while(gpio_level(CLOCK_PIN) == 1);
        code |= gpio_level(DATA_PIN)<<i;
        while(gpio_level(CLOCK_PIN) == 0);
      }

      io_read_char(code_to_ascii(code));

      irq2 &= ~GPIO0_IRQ;
      GPIO_REGS->event_detect_status.registers[0] = ~0;
    }
  }
}

void handle_invalid_irq(u64 elr_el1, u64 id, u64 esr_el1) {
  ERROR("\n\n[ERROR] Caught invalid interrupt. ID: %lu  elr_el1: %lu  esr_el1: %lu\n", id, elr_el1, esr_el1);
}