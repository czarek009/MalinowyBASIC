#ifndef  _PERIPHERIALS_H
#define  _PERIPHERIALS_H

#include "types.h"

#ifndef RPI_VERSION
#define RPI_VERSION 3
#endif

/* PBASE */
#if RPI_VERSION == 3
#define PBASE 0x3F000000
#define DEVICE_START 0x3B400000

#elif RPI_VERSION == 4
#define PBASE 0xFE000000
#define DEVICE_START 0xFC000000

#else
#define PBASE 0x00000000
#define DEVICE_START 0
#error PI_VERSION NOT DEFINED

#endif

/* GPIO */

#if RPI_VERSION == 3
#define NUM_OF_GPIO 53

#elif RPI_VERSION == 4
#define NUM_OF_GPIO 58

#endif

struct GpioPair {
  reg32 reserved;
  reg32 registers[2];
};

struct GpioRegisters {
  reg32 func_select[6];
  struct GpioPair set_output;
  struct GpioPair clear_output;
  struct GpioPair level;
  struct GpioPair event_detect_status;
  struct GpioPair rising_edge_detect_enable;
  struct GpioPair falling_edge_detect_enable;
  struct GpioPair high_level_detect_enable;
  struct GpioPair low_level_detect_enable;
  struct GpioPair asynchronous_rising_edge_detect_enable;
  struct GpioPair asynchronous_falling_edge_detect_enable;
  #if RPI_VERSION == 3
  reg32 reserved;
  reg32 pullup_pulldown_enable;
  reg32 pullup_pulldown_enable_clocks[2];
  #elif RPI_VERSION == 4
  reg32 reserved[22];
  reg32 pullup_pulldown[4];
  #endif
};

#define GPIO_REGS ((struct GpioRegisters *)(PBASE + 0x00200000))


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

/* IRQ */
struct IrqRegisters {
  reg32 irq0_pending_0;
  reg32 irq0_pending_1;
  reg32 irq0_pending_2;
  reg32 fiq_control;
  reg32 irq0_enable_1;
  reg32 irq0_enable_2;
  reg32 irq0_enable_0;
  reg32 res; // according to documentation, it shouldn't be here
  reg32 irq0_disable_1;
  reg32 irq0_disable_2;
  reg32 irq0_disable_0;
};

#define IRQ_REGS ((struct IrqRegisters *)(PBASE + 0x0000B200))

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