/// @file       stvec.c
/// @brief      RISC-V Test Monitor - S-mode Trap API and Vector Tables 

#include "arch/arch.h"
#include "tmon.h"

/* S-mode trap wrappers */

extern void _s_trap_wrapper(void);

/* S-mode ECALL handler (external, see smon.c) */

extern void s_exc_ecall(void *s);

/* S-mode default exception trap handlers */

static void s_exc_illegal_inst(void *s);
static void s_exc_fetch_fault (void *s);     
static void s_exc_load_fault  (void *s);    
static void s_exc_cross_fault (void *s);    
static void s_exc_store_fault (void *s);    

static void s_exc_default(void *s);
static void s_maj_default(void);
static void s_ext_default(void);

/* S-mode Software Trap Vector Table */

void *s_trap_vector[96] = {
    /* 0..31 - exception traps */
    (void*) s_exc_default,          // 0    - Instruction address misaligned
    (void*) s_exc_default,          // 1    - Instruction access fault
    (void*) s_exc_illegal_inst,     // 2    Illegal Instruction
    (void*) s_exc_default,          // 3    - Breakpoint
    (void*) s_exc_default,          // 4    - Load Address Misaligned
    (void*) s_exc_default,          // 5    - Load access fault
    (void*) s_exc_default,          // 6    - Store/AMO address misaligned
    (void*) s_exc_default,          // 7    - Store/AMO access fault

    (void*) s_exc_ecall,            // 8    U-mode ecall
    (void*) s_exc_ecall,            // 9    S-mode ecall
    (void*) s_exc_ecall,            // 10   V-mode ecall
    (void*) s_exc_default,          // 11   - M-mode ecall
    (void*) s_exc_fetch_fault,      // 12   MMU/MPU/PMP instruction fetch fault
    (void*) s_exc_load_fault,       // 13   MMU/MPU/PMP load fault
    (void*) s_exc_cross_fault,      // 14   ---/MPU/--- region crossing fault
    (void*) s_exc_store_fault,      // 15   MMU/MPU/PMP store/AMO fault
    
    (void*) s_exc_default,          // 16   reserved
    (void*) s_exc_default,          // 17   -- 
    (void*) s_exc_default,          // 18   
    (void*) s_exc_default,          // 19
    (void*) s_exc_default,          // 20
    (void*) s_exc_default,          // 21
    (void*) s_exc_default,          // 22
    (void*) s_exc_default,          // 23   -- 

    (void*) s_exc_default,          // 24   custom
    (void*) s_exc_default,          // 25   -- 
    (void*) s_exc_default,          // 26
    (void*) s_exc_default,          // 27
    (void*) s_exc_default,          // 28
    (void*) s_exc_default,          // 29
    (void*) s_exc_default,          // 30s    (void*) m_exc_default,          // 31   --

    /* 32..63 - major interrupts */
    
    (void*) s_maj_default,          // 32
    (void*) s_maj_default,          // 33
    (void*) s_maj_default,          // 34
    (void*) s_maj_default,          // 35
    (void*) s_maj_default,          // 36
    (void*) s_maj_default,          // 37
    (void*) s_maj_default,          // 38
    (void*) s_maj_default,          // 39

    (void*) s_maj_default,          // 40
    (void*) s_maj_default,          // 41
    (void*) s_maj_default,          // 42
    (void*) s_maj_default,          // 43
    (void*) s_maj_default,          // 44
    (void*) s_maj_default,          // 45
    (void*) s_maj_default,          // 46
    (void*) s_maj_default,          // 47

    (void*) s_maj_default,          // 48
    (void*) s_maj_default,          // 49
    (void*) s_maj_default,          // 50
    (void*) s_maj_default,          // 51
    (void*) s_maj_default,          // 52
    (void*) s_maj_default,          // 53
    (void*) s_maj_default,          // 54
    (void*) s_maj_default,          // 55

    (void*) s_maj_default,          // 56
    (void*) s_maj_default,          // 57
    (void*) s_maj_default,          // 58
    (void*) s_maj_default,          // 59
    (void*) s_maj_default,          // 60
    (void*) s_maj_default,          // 61
    (void*) s_maj_default,          // 62
    (void*) s_maj_default,          // 63
    
    /* 64..95 - external interrupt */
    
    (void*) s_ext_default,          // 64
    (void*) s_ext_default,          // 65
    (void*) s_ext_default,          // 66
    (void*) s_ext_default,          // 67
    (void*) s_ext_default,          // 68
    (void*) s_ext_default,          // 69
    (void*) s_ext_default,          // 70
    (void*) s_ext_default,          // 71

    (void*) s_ext_default,          // 72
    (void*) s_ext_default,          // 73
    (void*) s_ext_default,          // 74
    (void*) s_ext_default,          // 75
    (void*) s_ext_default,          // 76
    (void*) s_ext_default,          // 77
    (void*) s_ext_default,          // 78
    (void*) s_ext_default,          // 79

    (void*) s_ext_default,          // 80
    (void*) s_ext_default,          // 81
    (void*) s_ext_default,          // 82
    (void*) s_ext_default,          // 83
    (void*) s_ext_default,          // 84
    (void*) s_ext_default,          // 85
    (void*) s_ext_default,          // 86
    (void*) s_ext_default,          // 87

    (void*) s_ext_default,          // 88
    (void*) s_ext_default,          // 89
    (void*) s_ext_default,          // 90
    (void*) s_ext_default,          // 91
    (void*) s_ext_default,          // 92
    (void*) s_ext_default,          // 93
    (void*) s_ext_default,          // 94
    (void*) s_ext_default,          // 95
};


/******************************************************************************
** S-mode default trap handlers 
******************************************************************************/


/// @name   s_exc_default( *s )
/// @brief  default S-mode exception trap handler
static void s_exc_default(void *s) {

    register unsigned long *sf = (unsigned long *)s;

    ERROR("unexpected S-mode exception trap @PC=%lu\n", sf[17]);
    TRACE("scause: 0x%lx; sstatus: 0x%lx\n", sf[18], sf[16] );

    exit(-1);
}


/// @name   s_maj_default()
/// @brief  default S0mode major interrupt trap
static void s_maj_default(void) {

    ERROR("unexpected S-mode major interrupt trap\n");

    exit(-1);
}


/// @name   s_ext_default( )
/// @brief  default S-mode external interrupt trap
static void s_ext_default(void) {

    ERROR("unexpected S-mode external interrupt trap\n");

    exit(-1);
}

/******************************************************************************
** Named S-mode exception handlers 
******************************************************************************/


/// @name   void s_exc_illegal_inst( *s )
/// @brief  test monitor S-mode illegal instruction trap handler
void s_exc_illegal_inst(void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long tval;

    __csrr(tval, CSR_STVAL);

    if ( 2 == queue_pop() ) {

        TRACE("expected illegal instruction trap @0x%lx\n", tval);
        sf[17] += 4;                        // shift epc, skip faulty instruction
        return;
    }
    else {

        ERROR("unexpected illegal instruction trap @0x%lx\n", tval);
        exit(-1);
    } 

}


/// @name   s_exc_fetch_fault( *s )
/// @brief  test monitor S-mode illegal instruction trap handler
static void s_exc_fetch_fault(void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long tval;

    __csrr(tval, CSR_STVAL);

    if ( 12 == queue_pop() ) {

        TRACE("expected instruction fetch fault @0x%lx\n", tval);
        sf[17] = sf[0];                     // load ra to epc, skip faulty instruction
        return;
    }
    else {

        ERROR("unexpected instruction fetch fault @0x%lx\n", tval);
        exit(-1);
    }
}


/// @name   s_exc_load_fault( *s )
/// @brief  test monitor S-mode load fault exception trap handler
static void s_exc_load_fault (void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long tval;

    __csrr(tval, CSR_STVAL);

    if ( 13 == queue_pop() ) {

        TRACE("expected data load fault @0x%lx\n", tval);
        sf[17] += 4;                        // shift epc, skip faulty instruction
        return;

    }
    else {
        ERROR("unexpected data load fault @0x%lx\n", tval);
        exit(-1);
    }
}


/// @name   s_exc_cross_fault( *s )
/// @brief  test monitor S-mode MPU region crossing fault exception trap handler
static void s_exc_cross_fault(void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long tval;

    __csrr(tval, CSR_STVAL);

    if ( 14 == queue_pop() ) {

        TRACE("expected SMPU region crossing fault @0x%lx\n", tval);
        sf[17] += 4;                        // shift epc, skip faulty instruction
        return;

    }
    else {
        ERROR("unexpected SMPU region crossing fault @0x%lx\n", tval);
        exit(-1);
    }

}


/// @name   s_exc_store_fault( *s )
/// @brief  test monitor S-mode data store/AMO fault exception trap handler
static void s_exc_store_fault(void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long tval;

    __csrr(tval, CSR_STVAL);

    if ( 15 == queue_pop() ) {

        TRACE("expected data store fault @0x%lx\n", tval);
        sf[17] += 4;                        // shift epc, skip faulty instruction
        return;

    }
    else {

        ERROR("unexpected data store fault @0x%lx\n", tval);
        exit(-1);

    }
}


/******************************************************************************
** S-mode Trap API 
******************************************************************************/


/// @name
/// @brief set S-mode trap handling mode and vector 
/// @note  mode 1 needs reinitialize HW part of IVT, not supported yet!!!
int s_trap_mode(int mode) {

    register unsigned long stvec;

    switch (mode) {
        case 0:
            stvec = (unsigned long)(_s_trap_wrapper);
            break;
        case 1:
            return -1;
        case 3:
            stvec = (unsigned long)(&s_trap_vector[32]);
            stvec |= mode;
            break;
        default:
            return -1;
    }

    __csrw(CSR_STVEC, stvec);

    return 0;
}




/// @name   s_all_enable( enable )
/// @brief  Enable/disable S-mode interrupts (Sstatus.SIE)
int s_all_enable(int enable) {

    if (enable) 
        __csrs(CSR_SSTATUS, (1 << 1));
    else    
        __csrc(CSR_SSTATUS, (1 << 1));

    return enable;
}
