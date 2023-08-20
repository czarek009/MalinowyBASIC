#define S_FRAME_SIZE 256


.globl irq_init_vectors
irq_init_vectors:
  adr  x0, vectors
  msr  vbar_el1, x0
  ret

.globl irq_enable
irq_enable:
  msr  daifclr, #2
  ret

.globl irq_disable
irq_disable:
  msr  daifset, #2
  ret

.globl err_hang
err_hang:
  b  err_hang


.macro kernel_entry
  sub  sp, sp, #S_FRAME_SIZE
  stp  x0, x1,   [sp, #16 * 0]
  stp  x2, x3,   [sp, #16 * 1]
  stp  x4, x5,   [sp, #16 * 2]
  stp  x6, x7,   [sp, #16 * 3]
  stp  x8, x9,   [sp, #16 * 4]
  stp  x10, x11, [sp, #16 * 5]
  stp  x12, x13, [sp, #16 * 6]
  stp  x14, x15, [sp, #16 * 7]
  stp  x16, x17, [sp, #16 * 8]
  stp  x18, x19, [sp, #16 * 9]
  stp  x20, x21, [sp, #16 * 10]
  stp  x22, x23, [sp, #16 * 11]
  stp  x24, x25, [sp, #16 * 12]
  stp  x26, x27, [sp, #16 * 13]
  stp  x28, x29, [sp, #16 * 14]
  str  x30,      [sp, #16 * 15] 
.endm

.macro kernel_exit
  ldp  x0, x1,   [sp, #16 * 0]
  ldp  x2, x3,   [sp, #16 * 1]
  ldp  x4, x5,   [sp, #16 * 2]
  ldp  x6, x7,   [sp, #16 * 3]
  ldp  x8, x9,   [sp, #16 * 4]
  ldp  x10, x11, [sp, #16 * 5]
  ldp  x12, x13, [sp, #16 * 6]
  ldp  x14, x15, [sp, #16 * 7]
  ldp  x16, x17, [sp, #16 * 8]
  ldp  x18, x19, [sp, #16 * 9]
  ldp  x20, x21, [sp, #16 * 10]
  ldp  x22, x23, [sp, #16 * 11]
  ldp  x24, x25, [sp, #16 * 12]
  ldp  x26, x27, [sp, #16 * 13]
  ldp  x28, x29, [sp, #16 * 14]
  ldr  x30,      [sp, #16 * 15] 
  add  sp, sp, #S_FRAME_SIZE
  eret
.endm

.macro handle_invalid_interrupt
  mrs  x2, esr_el1
  mrs  x0, elr_el1
  bl   handle_invalid_irq
  b    err_hang
.endm

.macro  ventry  label
.align  7
    b  \label
.endm


.align  11
.globl vectors 
vectors:
  ventry  invalid_sync_el1t       // Synchronous EL1t
  ventry  invalid_irq_el1t        // IRQ EL1t
  ventry  invalid_fiq_el1t        // FIQ EL1t
  ventry  invalid_error_el1t      // Error EL1t
  ventry  invalid_sync_el1h       // Synchronous EL1h
  ventry  handle_el1_irq          // IRQ EL1h
  ventry  invalid_fiq_el1h        // FIQ EL1h
  ventry  invalid_error_el1h      // Error EL1h
  ventry  invalid_sync_el0_64     // Synchronous 64-bit EL0
  ventry  invalid_irq_el0_64      // IRQ 64-bit EL0
  ventry  invalid_fiq_el0_64      // FIQ 64-bit EL0
  ventry  invalid_error_el0_64    // Error 64-bit EL0
  ventry  invalid_sync_el0_32     // Synchronous 32-bit EL0
  ventry  invalid_irq_el0_32      // IRQ 32-bit EL0
  ventry  invalid_fiq_el0_32      // FIQ 32-bit EL0
  ventry  invalid_error_el0_32    // Error 32-bit EL0


invalid_sync_el1t:
  mov  x1, #0x0
  handle_invalid_interrupt

invalid_irq_el1t:
  mov  x1, #0x1
  handle_invalid_interrupt

invalid_fiq_el1t:
  mov  x1, #0x2
  handle_invalid_interrupt

invalid_error_el1t:
  mov  x1, #0x3
  handle_invalid_interrupt

invalid_sync_el1h:
  mov  x1, #0x4
  handle_invalid_interrupt

handle_el1_irq:
  kernel_entry
  bl  handle_irq
  kernel_exit

invalid_fiq_el1h:
  mov  x1, #0x6
  handle_invalid_interrupt

invalid_error_el1h:
  mov  x1, #0x7
  handle_invalid_interrupt

invalid_sync_el0_64:
  mov  x1, #0x8
  handle_invalid_interrupt

invalid_irq_el0_64:
  mov  x1, #0x9
  handle_invalid_interrupt

invalid_fiq_el0_64:
  mov  x1, #0xA
  handle_invalid_interrupt

invalid_error_el0_64:
  mov  x1, #0xB
  handle_invalid_interrupt

invalid_sync_el0_32:
  mov  x1, #0xC
  handle_invalid_interrupt

invalid_irq_el0_32:
  mov  x1, #0xD
  handle_invalid_interrupt

invalid_fiq_el0_32:
  mov  x1, #0xE
  handle_invalid_interrupt

invalid_error_el0_32:
  mov  x1, #0xF
  handle_invalid_interrupt
