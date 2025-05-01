/// @file   arch_regs.h
/// @brief  RISC-V RV32 registers definitions

#pragma once

#define BIT(n)  (1U << (n))

// Core Configuration and Status Registers (CSRs)

// Machine Trap Setup
#define CSR_MSTATUS         0x0300  // [MRW] Machine status register.
#define CSR_MISA            0x0301  // [MRW] ISA and extensions
#define CSR_MEDELEG         0x0302  // [MRW] Machine exception delegation register.
#define CSR_MIDELEG         0x0303  // [MRW] Machine interrupt delegation register.
#define CSR_MIE             0x0304  // [MRW] Machine interrupt-enable register.
#define CSR_MTVEC           0x0305  // [MRW] Machine trap-handler base address.
#define CSR_MCOUNTEREN      0x0306  // [MRW] Machine counter enable.
#define CSR_MSTATUSH        0x0310  // [MRW] Additional machine status register, RV32 only.
#define CSR_MIDELEGH		0x0313	// [MRW] Upper 32-bits of mideleg.
#define CSR_MIEH 			0x0314	// [MRW] Upper 32-bits of mie
// Machine Trap Handling
#define CSR_MSCRATCH        0x0340  // [MRW] Scratch register for machine trap handlers.
#define CSR_MEPC            0x0341  // [MRW] Machine exception program counter.
#define CSR_MCAUSE          0x0342  // [MRW] Machine trap cause.
#define CSR_MTVAL           0x0343  // [MRW] Machine bad address or instruction.
#define CSR_MIP             0x0344  // [MRW] Machine interrupt pending.
#define CSR_MTINST          0x034A  // [MRW] Machine trap instruction (transformed).
#define CSR_MTVAL2          0x034B  // [MRW] Machine bad guest physical address.
#define CSR_MIPH 			0x0354	// [MRW] Upper 32-bits of mip
#define CSR_MTOPI			0x0FB0	// [MRO] Machine top interrupt
#define CSR_MTOPEI			0x035C	// [MRW] Machine top external interrupt
// Machine Configuration
#define CSR_MENVCFG         0x030A  // [MRW] Machine environment configuration register.
#define CSR_MENVCFGH        0x031A  // [MRW] Additional machine env. conf. register, RV32 only.
#define CSR_MSECCFG         0x0747  // [MRW] Machine security configuration register.
#define CSR_MSECCFGH        0x0757  // [MRW] Additional machine security conf. register, RV32 only.
// Machine Non-Maskable Interrupt Handling
#define CSR_MNSCRATCH       0x0740  // [MRW] Resumable NMI scratch register.
#define CSR_MNEPC           0x0741  // [MRW] Resumable NMI program counter.
#define CSR_MNCAUSE         0x0742  // [MRW] Resumable NMI cause.
#define CSR_MNSTATUS        0x0744  // [MRW] Resumable NMI status.
// Machine Counter/Timers
#define CSR_MCYCLE          0x0b00  // [MRW] Machine cycle counter
#define CSR_MINSTRET        0x0b02  // [MRW] Machine instructions-retired counter
#define CSR_MCYCLEH         0x0b80  // [MRW] Upper 32 bits of MCYCLE, RV32 only
#define CSR_MINSTRETH       0x0b82  // [MRW] Upper 32 bits of MINSTRET, RV32 only.
// Machine Information Registers
#define CSR_MVENDORID       0x0F11  // [MRO] Vendor ID.
#define CSR_MARCHID         0x0F12  // [MRO] Architecture ID.
#define CSR_MIMPID          0x0F13  // [MRO] Implementation ID.
#define CSR_MHARTID         0x0F14  // [MRO] Hardware thread ID.
#define CSR_MCONFIGPTR      0x0F15  // [MRO] Pointer to configuration data structure.
// Machine Indirect CSR interface  
#define CSR_MISELECT		0x0350	// [MRW] Machine iCSR address register 
#define CSR_MIREG			0x0351	// [MRW] Machine iCSR data register

// Supervisor Trap Setup
#define CSR_SSTATUS         0x0100  // [SRW] Supervisor status register.
#define CSR_SIE             0x0104  // [SRW] Supervisor interrupt-enable register.
#define CSR_STVEC           0x0105  // [SRW] Supervisor trap-handler base address.
#define CSR_SCOUNTEREN      0x0106  // [SRW] Supervisor counter enable.
#define CSR_SIEH 			0x0114	// [SRW] Upper 32-bits of sie
// Supervisor Trap Handling
#define CSR_SSCRATCH        0x0140  // [SRW] Scratch register for supervisor trap handlers.
#define CSR_SEPC            0x0141  // [SRW] Supervisor exception program counter.
#define CSR_SCAUSE          0x0142  // [SRW] Supervisor trap cause.
#define CSR_STVAL           0x0143  // [SRW] Supervisor bad address or instruction.
#define CSR_SIP             0x0144  // [SRW] Supervisor interrupt pending.
#define CSR_SIPH 			0x0154	// [SRW] Upper 32-bits of sip


// Hypervisor Extended Supervisor Registers
#define CSR_HSTATUS			0x0600	// [SRW] Hypervisor Status


// S-MODE PMP (RISC-V SPMP)
#define spmpswitch0			0x170
#define spmpswitch1			0x171
#define CSR_SPMPSWITCH      0x170   // [SRW] base address of spmpswitch[2]
#define CSR_SPMPCFG         0x1A0   // [SRW] base address of spmpcfg[16]
#define CSR_SPMPADDR        0x1B0   // [SRW] base address of spmpaddr[64]


// S-MODE MPU (SNPS SMPU)
#define CSR_SISELECT		0x150	
#define CSR_SIREG			0x151	

#define CSR_VSISELECT		0x250
#define CSR_VSIREG			0x251

#define CSR_SMPUMASK		0x128	// Need to check for RV compliance 
#define CSR_HMPUMASK		0x620
#define CSR_VSMPUMASK       0x228

#define ICSR_SMPU_BASE		0x100	// L1 MPU
#define ICSR_HMPU_BASE		0x180	// L2 MPU


// MCAUSE/SCAUSE bit fields
#define CSR_xCAUSE_INT_BIT      31
#define CSR_xCAUSE_INT_MASK     BIT(CSR_xCAUSE_INT_BIT)

#define CSR_xCAUSE_EXC_INST_ACCESS_FAULT  1
#define CSR_xCAUSE_EXC_ILLEGAL_INST       2
#define CSR_xCAUSE_EXC_BREAKPOINT         3
#define CSR_xCAUSE_EXC_LOAD_ADDR_MISALIGN 4
#define CSR_xCAUSE_ECALL_FROM_U_EXC       8 /* The same for VU */
#define CSR_xCAUSE_ECALL_FROM_S_EXC       9 /* The same for HS */
#define CSR_xCAUSE_ECALL_FROM_VS_EXC      10
#define CSR_xCAUSE_ECALL_FROM_M_EXC       11
#define CSR_xCAUSE_EXC_SMPU_INST_FETCH    12
#define CSR_xCAUSE_EXC_SMPU_LOAD          13
#define CSR_xCAUSE_EXC_SMPU_MULTIPLE_TRANSLATIONS 14
#define CSR_xCAUSE_EXC_SMPU_STORE         15
#define CSR_xCAUSE_EXC_VIRTUAL_INST       22

#define CSR_xCAUSE_S_SW_INT     1
#define CSR_xCAUSE_M_SW_INT     3
#define CSR_xCAUSE_S_TMR_INT    5
#define CSR_xCAUSE_VS_TMR_INT   6
#define CSR_xCAUSE_M_TMR_INT    7
#define CSR_xCAUSE_S_EXT_INT    9
#define CSR_xCAUSE_VS_EXT_INT   10
#define CSR_xCAUSE_M_EXT_INT    11
#define CSR_xCAUSE_G_EXT_INT    12

// Major(local) interrupt source numbers
#define M_MODE_SOFT_INTERRUPT_NUM         CSR_xCAUSE_M_SW_INT
#define S_MODE_SOFT_INTERRUPT_NUM         CSR_xCAUSE_S_SW_INT

// MSTATUS/SSTATUS bit fields
#define CSR_xSTATUS_SPP_BIT     8
#define CSR_xSTATUS_SPP_SHIFT   CSR_xSTATUS_SPP_BIT
#define CSR_xSTATUS_SUM_BIT     18
#define CSR_xSTATUS_SUM_MASK    (1 << CSR_xSTATUS_SUM_BIT)

#define CSR_xSTATUS_SPIE_BIT    5
#define CSR_xSTATUS_SIE_BIT     1

// MSTATUS bit fields
#define CSR_MSTATUS_MPIE_BIT    7
#define CSR_MSTATUS_MIE_BIT     3
#define CSR_MSTATUS_SIE_BIT     CSR_xSTATUS_SIE_BIT

#define CSR_MSTATUS_MPP_SHIFT   11
#define CSR_MSTATUS_MPP_MASK    (BIT(CSR_MSTATUS_MPP_SHIFT) | BIT(CSR_MSTATUS_MPP_SHIFT + 1))

// MSTATUSH bit fields
#define CSR_MSTATUSH_MPV_SHIFT    7
#define CSR_MSTATUSH_MPV_MASK   (BIT(CSR_MSTATUSH_MPV_SHIFT))

// SSTATUS bit fields
#define CSR_SSTATUS_SPP_SHIFT   8
#define CSR_SSTATUS_SPP_MASK    (BIT(CSR_SSTATUS_SPP_SHIFT))

// VSTATUS bit fields
#define CSR_VSSTATUS_SPP_SHIFT   8
#define CSR_VSSTATUS_SPP_MASK    (BIT(CSR_VSSTATUS_SPP_SHIFT))

// HSTATUS bit fields
#define CSR_HSTATUS_SPV_SHIFT   7
#define CSR_HSTATUS_SPV_MASK    (BIT(CSR_HSTATUS_SPV_SHIFT))
#define CSR_HSTATUS_VGEIN_SHIFT 12
#define CSR_HSTATUS_VGEIN_MASK  (0x3F << CSR_HSTATUS_VGEIN_SHIFT)

// HSTATUS bit fields
#define CSR_HSTATUS_VGEIN_SHIFT 12
#define CSR_HSTATUS_VGEIN_MASK  (0x3F << CSR_HSTATUS_VGEIN_SHIFT)

// MIE bit definitions
#define MIE_SSIE            (1 <<  1)   // supervisor software interrupt enable
#define MIE_VSSIE           (1 <<  2)   // virtual supervisor software interrupt enable
#define MIE_MSIE            (1 <<  3)   // machine software interrupt enable
#define MIE_STIE            (1 <<  5)   // supervisor timer interrupt enable
#define MIE_VSTIE           (1 <<  6)   // virtual supervisor timer interrupt enable
#define MIE_MTIE            (1 <<  7)   // machine timer interrupt enable
#define MIE_SEIE            (1 <<  9)   // supervisor external interrupt enable
#define MIE_VSEIE           (1 << 10)   // virtual supervisor external interrupt enable
#define MIE_MEIE            (1 << 11)   // machine external interrupt enable
#define MIE_GEIE            (1 << 12)   // guest external interrupt enable
#define CSR_MIE_SSIE_BIT        1       // supervisor software interrupt
#define CSR_MIE_MSIE_BIT        3       // machine software interrupt
#define CSR_MIE_STIE_BIT        5       // supervisor timer interrupt enable
#define CSR_MIE_VSTIE_BIT       6       // virtual supervisor timer interrupt enable
#define CSR_MIE_MTIE_BIT        7       // machine timer interrupt enable
#define CSR_MIE_SEIE_BIT        9       // supervisor external interrupt

// SIE bit definitions

// HIE bit definitions
#define CSR_HIE_VSSIE_BIT      2
#define CSR_HIE_VSTIE_BIT      6
#define CSR_HIE_VSEIE_BIT      10
#define CSR_HIE_GEIE_BIT       12

// MIP bit definitions
#define MIP_SSIE            (1 <<  1)   // supervisor software interrupt pending
#define MIP_MSIE            (1 <<  3)   // machine software interrupt pending
#define MIP_STIE            (1 <<  5)   // supervisor timer interrupt pending
#define MIP_MTIE            (1 <<  7)   // machine timer interrupt pending
#define MIP_SEIE            (1 <<  9)   // supervisor external interrupt pending
#define MIP_VSEIE           (1 << 10)   // virtual supervisor external interrupt pending
#define MIP_MEIE            (1 << 11)   // machine external interrupt pending
#define CSR_MIP_MSIP_BIT        3
#define CSR_MIP_SSIP_BIT        1
#define CSR_MIP_STIP_BIT        5

// MTOPI bit definitions
#define CSR_MTOPI_IPRIO_SHIFT   0
#define CSR_MTOPI_IPRIO_MASK    0xFF
#define CSR_MTOPI_IID_SHIFT     16
#define CSR_MTOPI_IID_MASK      (0xFFF << CSR_MTOPI_IID_SHIFT)

// STOPI bit definitions
#define CSR_STOPI_IPRIO_SHIFT   0
#define CSR_STOPI_IPRIO_MASK    0xFF
#define CSR_STOPI_IID_SHIFT     16
#define CSR_STOPI_IID_MASK      (0xFFF << CSR_STOPI_IID_SHIFT)

// VSTOPI bit definitions
#define CSR_VSTOPI_IPRIO_SHIFT   0
#define CSR_VSTOPI_IPRIO_MASK    0xFF
#define CSR_VSTOPI_IID_SHIFT     16
#define CSR_VSTOPI_IID_MASK      (0xFFF << CSR_VSTOPI_IID_SHIFT)

// SIP bit definitions
#define SIP_STIP                BIT(5)  // supervisor timer interrupt pending
#define CSR_SIP_STIP_BIT        5
#define SIP_SEIP                BIT(9)  // supervisor external interrupt pending
#define SIP_VSEIP               BIT(10) // virtual supervisor external interrupt pending

// HIP bit definitions
#define HIP_VSEIP               BIT(10)
#define HIP_GEIP                BIT(12)

// HIP bit definitions
#define HIP_VSEIP               BIT(10)
#define HIP_GEIP                BIT(12)

// MIDELEG bit definitions
#define CSR_MIDELEG_SSI_BIT             1
#define CSR_MIDELEG_VSSI_BIT            2
#define CSR_MIDELEG_STI_BIT             5
#define CSR_MIDELEG_VSTI_BIT            6
#define CSR_MIDELEG_SEI_BIT             9
#define CSR_MIDELEG_VSEI_BIT            10
#define CSR_MIDELEG_GEI_BIT             12

// MEDELEG bit definitions
#define CSR_MEDELEG_ILL_INST_EXC_BIT    2

// MEDELEG bit definitions
#define CSR_MEDELEG_ILL_INST_EXC_BIT    2
#define CSR_MEDELEG_ECALL_U_BIT         8
#define CSR_MEDELEG_ECALL_S_BIT         9
#define CSR_MEDELEG_ECALL_VS_BIT        10

// HEDELEG bit definitions
#define CSR_HEDELEG_ILL_INST_EXC_BIT    2
#define CSR_HEDELEG_ECALL_U_BIT         8

// HIDELEG bit definitions
#define CSR_HIDELEG_VSSI_BIT            2
#define CSR_HIDELEG_VSTI_BIT            6
#define CSR_HIDELEG_VSEI_BIT            10

// MTVEC / STVEC bit definitions
#define CSR_xTVEC_VECTORED_MODE         BIT(0)
#define CSR_xTVEC_NESTED_VECTORED_MODE  (BIT(1) | BIT(0))

// MTVEC / STVEC bit definitions
#define CSR_xTVEC_VECTORED_MODE         BIT(0)
#define CSR_xTVEC_NESTED_VECTORED_MODE  (BIT(1) | BIT(0))

// xENVCFG bit definitions
#define CSR_xENVCFGH_STSP_BIT   8   // Supervisor-level Trap Stack Pointer Management
#define CSR_MENVCFGH_MTSP_BIT   9   // Machine-level Trap Stack Pointer Management
#define CSR_MENVCFGH_STCE_BIT   31  // STimecmp Enable. Enables stimecmp for S-mode when set to one

// HENVCFG bit definitions
#define CSR_HENVCFGH_HTSP_BIT   8   // Hypervisor Trap Stack Pointer HW swap enable
#define CSR_HENVCFGH_VGTSP_BIT  9   // Virtual Guest Trap Stack Pointer HW swap enable
#define CSR_HENVCFGH_STCE_BIT   31  // STimecmp Enable. Enables stimecmp for VS-mode when set to one

// MISA bit defenition
#define CSR_MISA_H_EXTENSION_BIT  7 // Hypervisor extension

// mcounteren, scounteren, hcounteren bit defenition
#define CSR_XCOUNTEREN_TM_BIT     1

// Memory Mapped Registers
#define PLIC_BASE               (0x0c000000)
#define PLIC_PRIORITY           (PLIC_BASE + 0x0)
#define PLIC_PENDING            (PLIC_BASE + 0x1000)
#define PLIC_MENABLE(hart)      (PLIC_BASE + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart)      (PLIC_BASE + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart)    (PLIC_BASE + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart)    (PLIC_BASE + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart)       (PLIC_BASE + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart)       (PLIC_BASE + 0x201004 + (hart)*0x2000)

#define MSIP_HART_0             (CONFIG_CLINT_BASE + 0x0000)
#define MSIP_HART_1             (CONFIG_CLINT_BASE + 0x0004)
#define MSIP_HART_2             (CONFIG_CLINT_BASE + 0x0008)
#define MSIP_HART_3             (CONFIG_CLINT_BASE + 0x000c)

#define MTIME_BASE              (CONFIG_CLINT_BASE + 0xbff8)
#define MTIME_LO                (MTIME_BASE + 0x0000)
#define MTIME_HI                (MTIME_BASE + 0x0004)
#define MTIME_CMP               (CONFIG_CLINT_BASE + 0x4000)

// IMSIC
#define IMSIC_HART0_M_IFILE                     ((CONFIG_IMSIC_M_HART0_BASE) + 0x0)
#define IMSIC_HART0_M_IFILE_SETEIPNUM_LE_OFFS   ((CONFIG_IMSIC_M_HART0_BASE) + 0x0)
#define IMSIC_HART0_S_IFILE                     ((CONFIG_IMSIC_S_HART0_BASE) + 0x0)
#define IMSIC_HART0_S_IFILE_SETEIPNUM_LE_OFFS   ((CONFIG_IMSIC_S_HART0_BASE) + 0x0)

#define IMSIC_IPRIO_REG(n)        (0x30 + ((n) >> 2))

#define M_EI_DELIVERY_REG         0x70
#define M_EI_THRESHOLD_REG        0x72
#define M_EI_PENDING_REG(x)       (0x80 + (x))
#define M_EI_ENABLE_REG(x)        (0xC0 + (x))

#define M_EI_DELIVERY_ENA         0x1

// APLIC
#define APLIC_HART0_BASE          0x40000000
#define APLIC_DOMAIN_OFFSET       0x10000

#define APLIC_M_DOMAIN_BASE       (APLIC_HART0_BASE + 0)
#define APLIC_S_DOMAIN_BASE       (APLIC_HART0_BASE + APLIC_DOMAIN_OFFSET)

#define APLIC_M_DOMAIN            0
#define APLIC_S_DOMAIN            1

#define APLIC_PAGE_BITS           12
#define APLIC_PAGE_SIZE           (1 << APLIC_PAGE_BITS)

#define APLIC_DOMAINCFG           0x0000
#define APLIC_SOURCECFG           0x0004
#define APLIC_MMSIADDRCFG         0x1BC0
#define APLIC_MMSIADDRCFGH        0x1BC4
#define APLIC_SMSIADDRCFG         0x1BC8
#define APLIC_SMSIADDRCFGH        0x1BCC
#define APLIC_SETIP               0x1C00
#define APLIC_SETIPNUM            0x1CDC
#define APLIC_IN_CLRIP            0x1D00
#define APLIC_CLRIPNUM            0x1DDC
#define APLIC_SETIE               0x1E00
#define APLIC_SETIENUM            0x1EDC
#define APLIC_CLRIE               0x1F00
#define APLIC_CLRIENUM            0x1FDC
#define APLIC_SETIPNUMLE          0x2000
#define APLIC_SETIPNUMBE          0x2004
#define APLIC_GENMSI              0x3000
#define APLIC_TARGET              0x3004

#define BIT(n)  (1U << (n))

/* Bit definitions */
#define APLIC_DOMAINCFG_BE_BIT              0ULL
#define APLIC_DOMAINCFG_BE_MASK             0x1
#define APLIC_DOMAINCFG_DM_BIT              2ULL
#define APLIC_DOMAINCFG_DM_MASK             0x1
#define APLIC_DOMAINCFG_IE_BIT              8ULL
#define APLIC_DOMAINCFG_IE_MASK             0x1
#define APLIC_DOMAINCFG_RO80_BIT            24ULL
#define APLIC_DOMAINCFG_RO80_MASK           0xFF

#define APLIC_SOURCECFG_CHILDIND_BIT        0ULL
#define APLIC_SOURCECFG_CHILDIND_MASK       0x3FF
#define APLIC_SOURCECFG_SM_BIT              0ULL
#define APLIC_SOURCECFG_SM_MASK             0x7
#define APLIC_SOURCECFG_D_BIT               10ULL
#define APLIC_SOURCECFG_D_MASK              0x1

enum source_mode {
	INACTIVE = 0, /* Inactive in this domain (and not delegated) */
	DETACHED = 1, /* Active, detached from the source wire */
	RESERVED1 = 2,
	RESERVED2 = 3,
	EDGE1 = 4, /* Active, edge-sensitive; interrupt asserted on rising edge */
	EDGE0 = 5, /* Active, edge-sensitive; interrupt asserted on falling edge */
	LEVEL1 = 6, /* Active, level-sensitive; interrupt asserted when high */
	LEVEL0 = 7 /* Active, level-sensitive; interrupt asserted when low */
};

#define APLIC_MMSIADDRCFGH_HIPPN_BIT        0ULL
#define APLIC_MMSIADDRCFGH_HIPPN_MASK       0xFFF
#define APLIC_MMSIADDRCFGH_LHXW_BIT         12ULL
#define APLIC_MMSIADDRCFGH_LHXW_MASK        0xF
#define APLIC_MMSIADDRCFGH_HHXW_BIT         16ULL
#define APLIC_MMSIADDRCFGH_HHXW_MASK        0x7
#define APLIC_MMSIADDRCFGH_LHXS_BIT         20ULL
#define APLIC_MMSIADDRCFGH_LHXS_MASK        0x7
#define APLIC_MMSIADDRCFGH_HHXS_BIT         24ULL
#define APLIC_MMSIADDRCFGH_HHXS_MASK        0x1F
#define APLIC_MMSIADDRCFGH_L_BIT            31ULL
#define APLIC_MMSIADDRCFGH_L_MASK           0x1

#define APLIC_SMSIADDRCFGH_HIPPN_BIT        0ULL
#define APLIC_SMSIADDRCFGH_HIPPN_MASK       0xFFF
#define APLIC_SMSIADDRCFGH_LHXS_BIT         20ULL
#define APLIC_SMSIADDRCFGH_LHXS_MASK        0x1FFF

#define APLIC_GENMSI_EIID_BIT               0ULL
#define APLIC_GENMSI_EIID_MASK              0x7FF
#define APLIC_GENMSI_BUSY_BIT               12ULL
#define APLIC_GENMSI_BUSY_MASK              0x1
#define APLIC_GENMSI_HARTIND_BIT            18ULL
#define APLIC_GENMSI_HARTIND_MASK           0x3FFF

#define APLIC_TARGETS_EIID_BIT              0ULL
#define APLIC_TARGETS_EIID_MASK             0x7FF
#define APLIC_TARGETS_RESERV_BIT            11ULL
#define APLIC_TARGETS_RESERV_MASK           0x1
#define APLIC_TARGETS_GUESTIND_BIT          12ULL
#define APLIC_TARGETS_GUESTIND_MASK         0x3F
#define APLIC_TARGETS_HARTIND_BIT           18ULL
#define APLIC_TARGETS_HARTIND_MASK          0x3FFF

#define mireg2      0x352
#define mireg3      0x353
#define mireg4      0x355
#define mireg5      0x356
#define mireg6      0x357

#define mtsp        0x7FF    // M-mode trap stack pointer swap register
#define stsp        0x5FF    // S-mode trap stack pointer swap register
#define htsp        0xAFF    // Hypervisor trap stack pointer swap register
#define vstsp       0x6FF    // VS-mode trap stack pointer swap register
