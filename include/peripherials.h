#ifndef  _PERIPHERIALS_H
#define  _PERIPHERIALS_H

#include "types.h"

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

#define GPFSEL0 (PBASE + 0x00200000)
#define GPFSEL1 (PBASE + 0x00200004)
#define GPFSEL2 (PBASE + 0x00200008)
#define GPFSEL3 (PBASE + 0x0020000C)
#define GPFSEL4 (PBASE + 0x00200010)
#define GPFSEL5 (PBASE + 0x00200014)

#define GPSET0 (PBASE + 0x0020001C)
#define GPSET1 (PBASE + 0x00200020)

#define GPCLR0 (PBASE + 0x00200028)
#define GPCLR1 (PBASE + 0x0020002C)

#define GPLEV0 (PBASE + 0x00200034)
#define GPLEV1 (PBASE + 0x00200038)

#define GPEDS0  (PBASE + 0x00200040)
#define GPEDS1  (PBASE + 0x00200044)

#define GPREN0  (PBASE + 0x0020004c)
#define GPREN1  (PBASE + 0x00200050)

#define GPFEN0  (PBASE + 0x00200058)
#define GPFEN1  (PBASE + 0x0020005c)

#define GPHEN0  (PBASE + 0x00200064)
#define GPHEN1  (PBASE + 0x00200068)

#define GPLEN0  (PBASE + 0x00200070)
#define GPLEN1  (PBASE + 0x00200074)

#define GPAREN0 (PBASE + 0x0020007c)
#define GPAREN1 (PBASE + 0x00200080)

#define GPAFEN0 (PBASE + 0x00200088)
#define GPAFEN1 (PBASE + 0x0020008c)

#if RPI_VERSION == 3
#define GPPUD (PBASE + 0x00200094)
#define GPPUDCLK0 (PBASE + 0x00200098)
#define GPPUDCLK1 (PBASE + 0x0020009C)
#define NUM_OF_GPIO 53

#elif RPI_VERSION == 4
#define GPIO_PUP_PDN_CNTRL_REG0 (PBASE + 0x002000E4)        /* Pin pull-up/down for pins 15:0  */
#define GPIO_PUP_PDN_CNTRL_REG1 (PBASE + 0x002000E8)        /* Pin pull-up/down for pins 31:16 */
#define GPIO_PUP_PDN_CNTRL_REG2 (PBASE + 0x002000EC)        /* Pin pull-up/down for pins 47:32 */
#define GPIO_PUP_PDN_CNTRL_REG3 (PBASE + 0x002000F0)        /* Pin pull-up/down for pins 57:48 */
#define NUM_OF_GPIO 58

#endif

/* AUX */
struct AuxRegisters {
  reg32 irq_status;
  reg32 enables;
  reg32 reserved1[14];
  reg32 mu_io;
  reg32 mu_ier;
  reg32 mu_iir;
  reg32 mu_lcr;
  reg32 mu_mcr;
  reg32 mu_lsr;
  reg32 mu_msr;
  reg32 mu_scratch;
  reg32 mu_control;
  reg32 mu_status;
  reg32 mu_baud_rate;
  reg32 reserved2[5];
#if RPI_VERSION == 3
  reg32 spi1_control0;
  reg32 spi1_control1;
  reg32 spi1_status;
  reg32 spi1_data;
  reg32 spi1_peek;
  reg32 spi2_control0;
  reg32 spi2_control1;
  reg32 spi2_status;
  reg32 spi2_data;
  reg32 spi2_peek;
#elif RPI_VERSION == 4
  reg32 spi1_control0;
  reg32 spi1_control1;
  reg32 spi1_status;
  reg32 spi1_peek;
  reg32 spi1_data_a;
  reg32 spi1_data_b;
  reg32 spi1_data_c;
  reg32 spi1_data_d;
  reg32 spi1_ex_data_a;
  reg32 spi1_ex_data_b;
  reg32 spi1_ex_data_c;
  reg32 spi1_ex_data_d;
  reg32 spi2_control0;
  reg32 spi2_control1;
  reg32 spi2_status;
  reg32 spi2_peek;
  reg32 spi2_data_a;
  reg32 spi2_data_b;
  reg32 spi2_data_c;
  reg32 spi2_data_d;
  reg32 spi2_ex_data_a;
  reg32 spi2_ex_data_b;
  reg32 spi2_ex_data_c;
  reg32 spi2_ex_data_d;
#endif
};

// #define AUX_OFFSET 0x00215000
#define AUX_REGS ((struct AuxRegisters *)(PBASE + 0x00215000))

#endif  /*_PERIPHERIALS_H */