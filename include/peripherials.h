#ifndef  _PERIPHERIALS_H
#define  _PERIPHERIALS_H

#ifndef RPI_VERSION
#define RPI_VERSION 3
#endif

/* PBASE */
#if RPI_VERSION == 3
#define PBASE 0x3F000000

#elif RPI_VERSION == 4
#define PBASE 0xFE000000

#else
#define PBASE 0x00000000
#error PI_VERSION NOT DEFINED

#endif

/* GPIO */

#define GPFSEL1 (PBASE + 0x00200004)

#define GPIO_PUP_PDN_CNTRL_REG0 (PBASE + 0x002000E4) /* Pin pull-up/down for pins 15:0  */

/* AUX */
#define AUX_ENABLES     (PBASE + 0x00215004)
#define AUX_MU_IO_REG   (PBASE + 0x00215040)
#define AUX_MU_IER_REG  (PBASE + 0x00215044)
#define AUX_MU_IIR_REG  (PBASE + 0x00215048)
#define AUX_MU_LCR_REG  (PBASE + 0x0021504C)
#define AUX_MU_MCR_REG  (PBASE + 0x00215050)
#define AUX_MU_LSR_REG  (PBASE + 0x00215054)
#define AUX_MU_CNTL_REG (PBASE + 0x00215060)
#define AUX_MU_BAUD_REG (PBASE + 0x00215068)

/* IRQ */
#define IRQ0_PENDING_0 (PBASE + 0x0000B200)
#define IRQ0_PENDING_1 (PBASE + 0x0000B204)
#define IRQ0_PENDING_2 (PBASE + 0x0000B208)
#define FIQ_CONTROL    (PBASE + 0x0000B20C)
#define IRQ0_ENABLE_1  (PBASE + 0x0000B210)
#define IRQ0_ENABLE_2  (PBASE + 0x0000B214)
#define IRQ0_ENABLE_0  (PBASE + 0x0000B218)
#define RES            (PBASE + 0x0000B21C)
#define IRQ0_DISABLE_1 (PBASE + 0x0000B220)
#define IRQ0_DISABLE_2 (PBASE + 0x0000B224)
#define IRQ0_DISABLE_0 (PBASE + 0x0000B228)


#endif  /*_PERIPHERIALS_H */