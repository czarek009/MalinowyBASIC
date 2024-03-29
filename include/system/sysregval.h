#ifndef _SYSREGVAL_H
#define _SYSREGVAL_H

#define SCTLR_RESERVED            (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN    (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN   (0 << 24)
#define SCTLR_I_CACHE_DISABLED    (0 << 12)
#define SCTLR_D_CACHE_DISABLED    (0 << 2)
#define SCTLR_MMU_DISABLED        (0 << 0)
#define SCTLR_MMU_ENABLED         (1 << 0)
#define SCTLR_VALUE_MMU_DISABLED  (SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

#define HCR_VAL (1<<31)

#define SCR_RESERVED  (3 << 4)
#define SCR_RW        (1 << 10)
#define SCR_NS        (1 << 0)
#define SCR_VAL       (SCR_RESERVED | SCR_RW | SCR_NS)

#define SPSR_MASK_ALL  (7 << 6)
#define SPSR_EL1h      (5 << 0)
#define SPSR_VAL       (SPSR_MASK_ALL | SPSR_EL1h)

#define TCR_TG1_4K     (2 << 30)
#define TCR_T1SZ       ((64 - 48) << 16)
#define TCR_TG0_4K     (0 << 14)
#define TCR_T0SZ       (64 - 48)
#define TCR_EL1_VAL    (TCR_TG1_4K | TCR_T1SZ | TCR_TG0_4K | TCR_T0SZ)

#define CPACR_EL1_FPEN    (1 << 21) | (1 << 20)
#define CPACR_EL1_ZEN     (1 << 17) | (1 << 16)
#define CPACR_EL1_VAL     (CPACR_EL1_FPEN | CPACR_EL1_ZEN)

#define MT_DEVICE_nGnRnE        0x0
#define MT_NORMAL_NC            0x1
#define MT_DEVICE_nGnRnE_FLAGS  0x00
#define MT_NORMAL_NC_FLAGS      0x44
#define MAIR_VALUE              (MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC))

#endif