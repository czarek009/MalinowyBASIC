#ifndef _IRQ_H
#define _IRQ_H

#define S_FRAME_SIZE 256

#define AUX_IRQ          (1 << 29)
#define GPIO0_IRQ        (1 << 17)

void enable_interrupt_controller(void);
void irq_init_vectors(void);
void irq_enable(void);
void irq_disable(void);
void irq_startup_info(void);

#endif /* _IRQ_H */