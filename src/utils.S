#include "sysregval.h"

.globl put32
put32:
  str  w1, [x0]
  ret

.globl get32
get32:
  ldr  w0, [x0]
  ret

.globl delay
delay:
  subs  x0, x0, #1
  bne   delay
  ret

.globl get_el
get_el:
  mrs  x0, CurrentEL
  lsr  x0, x0, #2
  ret

// This function assumes that current el == 3
.globl set_el1
set_el1:
  ldr  x0, =SCTLR_VALUE_MMU_DISABLED
  msr  sctlr_el1, x0

  ldr  x0, =HCR_VAL
  msr  hcr_el2, x0

  ldr  x0, =SCR_VAL
  msr  scr_el3, x0

  ldr  x0, =SPSR_VAL
  msr  spsr_el3, x0

  adr  x0, el1_entry
  msr  elr_el3, x0

  eret

el1_entry:
  ret