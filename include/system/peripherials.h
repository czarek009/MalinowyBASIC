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

/* RNG */
#if RPI_VERSION == 3

struct RngRegisters{
  reg32 rng_ctrl;
  reg32 rng_status;
  reg32 rng_data;
  reg32 rng_int_mask;
};

#elif RPI_VERSION == 4

struct RngRegisters{
  reg32 rng_ctrl;
  u8 reserved0[12];
  reg32 rng_bit_count_threshold;
  u8 reserved1[12];
  reg32 rng_data;
  reg32 rng_fifo_count;
};

#define RNG_FIFO_COUNT_THRESHOLD_SHIFT  8
#define RNG_DIV_CTRL_SHIFT              13
#define RNG_CTRL_RBGEN_MASK             0x00001FFF
#define RNG_FIFO_COUNT_MASK             0x000000FF
#endif

#define RNG_REGS ((struct RngRegisters *)(PBASE + 0x00104000))

/* TIMER */
struct TimerRegisters {
  reg32 timer_cs;
  reg32 timer_clo;
  reg32 timer_chi;
  reg32 timer_c0;
  reg32 timer_c1;
  reg32 timer_c2;
  reg32 timer_c3;
};

#define TIMER_REGS ((struct TimerRegisters *)(PBASE + 0x00003000))

/* MAILBOX */
struct MailboxRegisters {
  reg32 read;
  reg32 reserved[5];
  reg32 status;
  reg32 config;
  reg32 write;
};

#define MBX_REGS ((struct MailboxRegisters *)(PBASE + 0xB880))

/* EMMC */
struct EmmcRegisters {
  reg32 arg2;
  reg32 block_size_count;
  reg32 arg1;
  reg32 cmd_xfer_mode;
  reg32 response[4];
  reg32 data;
  reg32 status;
  reg32 control[2];
  reg32 int_flags;
  reg32 int_mask;
  reg32 int_enable;
  reg32 control2;
  reg32 cap1;
  reg32 cap2;
  reg32 res0[2];
  reg32 force_int;
  reg32 res1[7];
  reg32 boot_timeout;
  reg32 debug_config;
  reg32 res2[2];
  reg32 ext_fifo_config;
  reg32 ext_fifo_enable;
  reg32 tune_step;
  reg32 tune_SDR;
  reg32 tune_DDR;
  reg32 res3[23];
  reg32 spi_int_support;
  reg32 res4[2];
  reg32 slot_int_status;
};

#if RPI_VERSION == 3
#define EMMC_OFFSET 0x00300000
#else
#define EMMC_OFFSET 0x00340000
#endif

#define EMMC_REGS ((struct EmmcRegisters *)(PBASE + EMMC_OFFSET))

#endif  /*_PERIPHERIALS_H */