#include "mm.h"
#include "sysregval.h"

.section ".text.boot"

.globl _start
_start:
  mrs  x0, mpidr_el1
  and  x0, x0, #0xFF
  cbz  x0, main_cpu  // mpidr_el1 == 0  <=>  main cpu
  cmp  x0, #0x1      // mpidr_el1 == 1  <=>  hdmi cpu
  bne  proc_hang

  mov sp, #MINI_STACK_ADDRESS

  bl   hdmi_cpu
  b    proc_hang

main_cpu:
enable_fpu:
  mov  x0, #0x33ff
  msr  cptr_el3, x0

  mrs  x1, cpacr_el1
  mov  x0, #(3 << 20)
  orr  x0, x1, x0
  msr  cpacr_el1, x0

set_el1:
  ldr  x0, =SCTLR_VALUE_MMU_DISABLED
  msr  sctlr_el1, x0

  ldr  x0, =HCR_VAL
  msr  hcr_el2, x0

  ldr  x0, =SCR_VAL
  msr  scr_el3, x0

  ldr  x0, =SPSR_VAL
  msr  spsr_el3, x0

activate_mmu:
  ldr  x0, =CPACR_EL1_VAL
  msr  cpacr_el1, x0

  ldr  x0, =TCR_EL1_VAL
  msr  tcr_el1, x0

  ldr  x0, =MAIR_VALUE
  msr  mair_el1, x0

  adr  x0, master
  msr  elr_el3, x0

  eret

master:
  adr  x0, bss_begin
  adr  x1, bss_end
  sub  x1, x1, x0
  bl   memzero

  mov  sp, #STACK_ADDRESS

  bl init_mmu

  adrp x0, id_pgd
  msr ttbr0_el1, x0

  mrs x0, sctlr_el1
  mov x1, #SCTLR_MMU_ENABLED
  orr x0, x0, x1
  msr sctlr_el1, x0

  bl   main
  b    proc_hang

proc_hang:
  b   proc_hang

.globl id_pgd_addr
id_pgd_addr:
    adrp x0, id_pgd
    ret