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

#define GPFSEL1 (PBASE + 0x00200004)

#define GPIO_PUP_PDN_CNTRL_REG0 (PBASE + 0x002000E4) /* Pin pull-up/down for pins 15:0  */

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