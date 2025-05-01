/// @file       mtvec.c
/// @brief      RISC-V Test Monitor - M-mode Trap API and Vector Tables 

#include "arch/arch.h"
#include "tmon.h"

/* M-mode trap wrappers */

extern void _m_trap_wrapper(void);

extern void _m_exc_wrapper(void);
extern void _m_nvi_wrapper(void);
extern void _m_chi_wrapper(void);

/* M-mode ECALL handler (external, see mmon.c) */

extern void m_exc_ecall(void *s);

/* Default M-mode trap handlers */

static void m_exc_illegal_inst(void *s);
static void m_exc_fetch_fault (void *s);     
static void m_exc_load_fault  (void *s);    
static void m_exc_cross_fault (void *s);    
static void m_exc_store_fault (void *s);    

static void m_exc_default(void *s);
static void m_maj_default(void *s);
static void m_ext_default(void *s);
       void m_nvi_default(void *s);


static void m_maj_ext_wrapper(void *s);

/* M-mode Software Trap Vector Table */

void *m_trap_vector[96] = {
    /* 0..31 - exception traps */
    (void*) m_exc_default,          // 0    - Instruction address misaligned
    (void*) m_exc_default,          // 1    - Instruction access fault
    (void*) m_exc_illegal_inst,     // 2    Illegal Instruction
    (void*) m_exc_default,          // 3    - Breakpoint
    (void*) m_exc_default,          // 4    - Load Address Misaligned
    (void*) m_exc_default,          // 5    - Load access fault
    (void*) m_exc_default,          // 6    - Store/AMO address misaligned
    (void*) m_exc_default,          // 7    - Store/AMO access fault

    (void*) m_exc_ecall,            // 8    U-mode ecall
    (void*) m_exc_ecall,            // 9    S-mode ecall
    (void*) m_exc_ecall,            // 10   V-mode ecall
    (void*) m_exc_ecall,            // 11   M-mode ecall
    (void*) m_exc_fetch_fault,      // 12   MMU/MPU/PMP instruction fetch fault
    (void*) m_exc_load_fault,       // 13   MMU/MPU/PMP load fault
    (void*) m_exc_cross_fault,      // 14   ---/MPU/--- region crossing fault
    (void*) m_exc_store_fault,      // 15   MMU/MPU/PMP store/AMO fault
    
    (void*) m_exc_default,          // 16   reserved
    (void*) m_exc_default,          // 17   -- 
    (void*) m_exc_default,          // 18   
    (void*) m_exc_default,          // 19
    (void*) m_exc_default,          // 20
    (void*) m_exc_default,          // 21
    (void*) m_exc_default,          // 22
    (void*) m_exc_default,          // 23   -- 

    (void*) m_exc_default,          // 24   custom
    (void*) m_exc_default,          // 25   -- 
    (void*) m_exc_default,          // 26
    (void*) m_exc_default,          // 27
    (void*) m_exc_default,          // 28
    (void*) m_exc_default,          // 29
    (void*) m_exc_default,          // 30
    (void*) m_exc_default,          // 31   --

    /* 32..63 - major interrupts */
    
    (void*) m_maj_default,          // 32 maj0  unused
    (void*) m_maj_default,          // 33 maj1  S-mode software interrupt
    (void*) m_maj_default,          // 34 maj2  VS-mode software interrupt, always delegated
    (void*) m_maj_default,          // 35 maj3  M-mode software interrupt
    (void*) m_maj_default,          // 36 maj4  unused 
    (void*) m_maj_default,          // 37 maj5  S-mode timer interrupt
    (void*) m_maj_default,          // 38 maj6  VS-mode timer interrupt, always delegated
    (void*) m_maj_default,          // 39 maj7  M-mode time interrupt

    (void*) m_maj_default,          // 40 maj8  unused 
    (void*) m_maj_default,          // 41 maj9  S-mode external interrupt
    (void*) m_maj_default,          // 42 maj10 VS-mode external interrupt, always delegated
    (void*) m_maj_ext_wrapper,      // 43 maj11 M-mode external interrupt 
    (void*) m_maj_default,          // 44 maj12 Guest external interrupt
    (void*) m_maj_default,          // 45 maj13 Counter overflow interrupt
    (void*) m_maj_default,          // 46 maj14 
    (void*) m_maj_default,          // 47 maj15

    (void*) m_maj_default,          // 48
    (void*) m_maj_default,          // 49
    (void*) m_maj_default,          // 50
    (void*) m_maj_default,          // 51
    (void*) m_maj_default,          // 52
    (void*) m_maj_default,          // 53
    (void*) m_maj_default,          // 54
    (void*) m_maj_default,          // 55

    (void*) m_maj_default,          // 56
    (void*) m_maj_default,          // 57
    (void*) m_maj_default,          // 58
    (void*) m_maj_default,          // 59
    (void*) m_maj_default,          // 60
    (void*) m_maj_default,          // 61
    (void*) m_maj_default,          // 62
    (void*) m_maj_default,          // 63
    
    /* 64..95 - external interrupt */
    
    (void*) m_ext_default,          // 64
    (void*) m_ext_default,          // 65
    (void*) m_ext_default,          // 66
    (void*) m_ext_default,          // 67
    (void*) m_ext_default,          // 68
    (void*) m_ext_default,          // 69
    (void*) m_ext_default,          // 70
    (void*) m_ext_default,          // 71

    (void*) m_ext_default,          // 72
    (void*) m_ext_default,          // 73
    (void*) m_ext_default,          // 74
    (void*) m_ext_default,          // 75
    (void*) m_ext_default,          // 76
    (void*) m_ext_default,          // 77
    (void*) m_ext_default,          // 78
    (void*) m_ext_default,          // 79

    (void*) m_ext_default,          // 80
    (void*) m_ext_default,          // 81
    (void*) m_ext_default,          // 82
    (void*) m_ext_default,          // 83
    (void*) m_ext_default,          // 84
    (void*) m_ext_default,          // 85
    (void*) m_ext_default,          // 86
    (void*) m_ext_default,          // 87

    (void*) m_ext_default,          // 88
    (void*) m_ext_default,          // 89
    (void*) m_ext_default,          // 90
    (void*) m_ext_default,          // 91
    (void*) m_ext_default,          // 92
    (void*) m_ext_default,          // 93
    (void*) m_ext_default,          // 94
    (void*) m_ext_default,          // 95
};

/* M-mode Trap User Callback Vector */

void *m_trap_callback[96] = {};


/******************************************************************************
** M-mode default trap handlers 
******************************************************************************/

/// @name   m_exc_default(*s)
/// @brief  default M-mode exception handler
static void m_exc_default(void *s) {

    register unsigned long *sf      = (unsigned long *)s;
    register unsigned long mcause   = sf[18];

    if ( mcause == queue_pop() ) {
        TRACE("expected M-mode exception trap #0x%lx\n", mcause);
        if ( 0 != m_trap_callback[mcause] ) {
            ((void (*)(void*))(m_trap_callback[mcause]))(s);   
        }
        else {
            WARNING("no user callback assigned to trap #%ld\n", mcause);
        }
    }
    else {
        ERROR("unexpected M-mode exception trap #%ld @PC=%lu\n", mcause, sf[17]);
        TRACE("mcause: 0x%lx; mstatus: 0x%lx\n", sf[18], sf[16] );
        exit(-1);
    }

}


/// @name   m_maj_default( *s )
/// @brief  default M-mode major interrupt handler
static void m_maj_default(void *s) {

    register unsigned long *sf      = (unsigned long *)s;
    register unsigned long mtopi, iid, expect; 

    __csrr(mtopi, CSR_MTOPI);
    iid = mtopi >> 16;
    expect = queue_pop();

    if ( (32 + iid) == expect ) {
        TRACE("expected major interrupt trap iid=%ld\n", iid);
        if ( 0 != m_trap_callback[expect] ) {
            ((void (*)(void*s))(m_trap_callback[expect]))(s);   
        }
        else {
            WARNING("no user callback assigned to trap #%ld\n", expect);
        }
    }
    else {
        ERROR("unexpected M-mode major interrupt trap #%ld, expected #%ld\n", iid, expect - 32);   
        TRACE("mcause: 0x%lx; mstatus: 0x%lx\n", sf[18], sf[16] );
        exit(-1);
    }
}


/// @name   m_ext_default( *s )
/// @brief  default M-mode external interrupt handler
static void m_ext_default(void *s) {

    register unsigned long *sf      = (unsigned long *)s;
    register unsigned long eiid     = sf[20];
    register unsigned long expect   = queue_pop();

    if ( (64 + (eiid >> 16)) == expect ) {
        TRACE("expected external interrupt trap eiid=%ld\n", eiid >> 16);
        if ( 0 != m_trap_callback[expect] ) {
            ((void (*)(void*s))(m_trap_callback[expect]))(s);   
        }
        else {
            WARNING("default external interrupt handler with no user callback assigned to #%ld\n", expect);
        }
    }
    else {
        ERROR("unexpected external interrupt trap #%ld, expect %ld\n", eiid >> 16, expect - 64);
        TRACE("mcause: 0x%lx; epc: 0x%lx\n", sf[18], sf[17] );
        exit(-1);
    }

}


/// @name  m_nvi_handler( *s )
/// @brief Test monitor M-mode nested interrupt handler 
void m_nvi_default(void *s) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long expect = queue_pop(); 
    register unsigned long eiid; 

    __csrw(CSR_MISELECT, M_EI_THRESHOLD_REG);
    __csrrw(eiid, CSR_MIREG, 0);

    sf[20] = eiid;              // store current eiid to the trap stack frame
                                // and make it visible to the next level 

    if ( (32 + eiid) == expect ) {
        TRACE("expected external interrupt trap eiid=%ld\n", eiid);
        if ( 0 != m_trap_callback[expect] ) {
            ((void (*)(void*s))(m_trap_callback[expect]))(s);   
        }
        else {
            WARNING("default external interrupt handler with no user callback assigned to #%ld\n", expect);
        }        
    } 
    else {
        ERROR("unexpected external interrupt trap #%ld, expect %ld\n", eiid, expect - 32);
        TRACE("pc: 0x%lx; status: %lx\n", sf[17], sf[16] );

        exit(-1);
    }

}


/// @name   m_maj_ext_wrapper( *s )
/// @brief  default M-mode major external interrupt handler, mtvec.MODE=0,1 
static void m_maj_ext_wrapper(void *s) {

    register unsigned long *sf      = (unsigned long *)s;
    register unsigned long topei;

    // read top eiid and claim it
    asm volatile ("csrrw %0, mtopei, zero" : "=r"(topei) :: );

    sf[20] = topei;     // store topei to the trap stack frame, 
                        // so it is visible at the next level 
 
    return ((void (*)(void*))(m_trap_vector[64 + (topei >> 16)]))(s);

}


/******************************************************************************
** Named M-mode exception handlers 
******************************************************************************/


/// @name   void m_exc_illegal_inst( *s )
/// @brief  test monitor M-mode illegal instruction trap handler
static void m_exc_illegal_inst(void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long tval;

    __csrr(tval, CSR_MTVAL);

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


/// @name   void m_exc_fetch_fault( *s )
/// @brief
static void m_exc_fetch_fault(void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long tval;

    __csrr(tval, CSR_MTVAL);

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


/// @name   m_exc_load_fault( *s )
/// @brief
static void m_exc_load_fault (void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long tval;

    __csrr(tval, CSR_MTVAL);

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


/// @name   m_exc_cross_fault( *s )
/// @brief
static void m_exc_cross_fault(void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long tval;

    __csrr(tval, CSR_MTVAL);

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


/// @name   m_exc_store_fault( *s )
/// @brief
static void m_exc_store_fault(void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    register unsigned long tval;

    __csrr(tval, CSR_MTVAL);

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
** M-mode Trap API 
******************************************************************************/


/// @name  m_trap_mode(mode)
/// @brief set trap handling mode and (test monitor specific) vector 
/// @note  mode 1 needs reinitialize HW part of IVT, not supported yet!!!
int m_trap_mode(int mode) {

    register unsigned long mtvec;

    switch (mode) {
        case 0:
            mtvec = (unsigned long)(_m_trap_wrapper);
            break;
        case 1:
            return -1;
        case 3:
            m_trap_vector[32] = (void*)_m_exc_wrapper;
            for (int i = 33; i < 96; i++)
                m_trap_vector[i] = (void*)_m_nvi_wrapper;
            mtvec = (unsigned long)(&m_trap_vector[32]);
            mtvec |= mode;
            break;
        default:
            return -1;
    }

    __csrw(CSR_MTVEC, mtvec);

    return 0;
}


/// @name   m_exc_setvec(eid, *handler)
/// @brief  add exception trap handler entry to vector table 
///         (depends on configured trap handling mode)
int m_exc_setvec(int eid, void *handler) {

    register unsigned long mtvec;

    __csrr(mtvec, CSR_MTVEC);

    switch (mtvec & 0x03) {
        case 0:
        case 3:
            m_trap_vector[eid] = handler;
            break;
        case 1:
        // not supported yet
        default:
            ERROR("setting trap vector for mtvec.MODE=%ld is not implemented\n", mtvec & 0x03);
            exit(-1);
    }


    return 0;
}


/// @name   m_maj_setvec(iid, *handler)
/// @brief  add major interrupt trap handler entry to vector table
///         (depends on configured trap handling mode)
int m_maj_setvec(int iid, void *handler) {

    register unsigned long mtvec;

    __csrr(mtvec, CSR_MTVEC);

    switch (mtvec & 0x03) {
        case 0:
            m_trap_vector[32 + iid] = handler;
            break;
        case 3:
            ERROR("not supported for major interrupts in nested vectored mode. use m_ext_setvec() instead\n");
            exit(-1);
        case 1:
        // not supported yet
        default:
            ERROR("not yet implemented\n");
            exit(-1);
    }

    return 0;
}


/// @name   m_ext_setvec(eiid, *handler)
/// @brief  add external interrupt trap handler entry to vector table
///         (depends on configured trap handling mode)
int m_ext_setvec(int eiid, void *handler) {

    register unsigned long mtvec;

    __csrr(mtvec, CSR_MTVEC);

    switch (mtvec & 0x03) {
        case 0:
            m_trap_vector[32 + 32 + eiid] = handler;
            break;
        case 1:
        // not yet supported
        case 3:
        // not yet supported
        default:
            WARNING("not yet implemented\n");
            return -1;
    }

    return 0;
}


/// @name   m_exc_delegate(eid, delegate)
/// @brief  enable/disable M-mode exception trap delegation
int m_exc_delegate(int eid, int delegate) {


    if (delegate)
        __csrs(CSR_MEDELEG, (1 << eid));
    else
        __csrc(CSR_MEDELEG, (1 << eid));

    return 0;
}


/// @name   m_maj_delegate(iid, delegate)
/// @brief  enable/disable M-mode major interrupt delegation
int m_maj_delegate(int iid, int delegate) {

    if (delegate)
        if (iid < 32) 
            __csrs(CSR_MIDELEG, 1 << iid);
        else 
            __csrs(CSR_MIDELEGH, 1 << (iid >> 1));
    else
        if (iid < 32)
            __csrc(CSR_MIDELEG, 1 << iid);
        else 
            __csrc(CSR_MIDELEGH, 1 << (iid >> 1));

    return 0;
}


/// @name   m_maj_priority(iid, priority)
/// @brief  set major interrupt priority
int m_maj_priority(int iid, int priority) {

    register unsigned long ireg = 0x30 + (iid >> 2);
    register unsigned long ival;

    __csrw(CSR_MISELECT, ireg);
    __csrr(ival, CSR_MIREG);

    ival &= ~(0xff << ((iid % 4) * 8));
    ival |= (priority << ((iid % 4) * 8));

    __csrw(CSR_MIREG, ival);

    return 0;
}


/// @name   m_maj_enable(iid, enable)
/// @brief  enable/disable major interrupt
int m_maj_enable(int iid, int enable) {

    if (enable)
        if (iid < 32)
            __csrs(CSR_MIE, 1 << iid);
        else    
            __csrs(CSR_MIEH, 1 << (iid >> 1));
    else
        if (iid < 32)
            __csrc(CSR_MIE, 1 << iid);
        else    
            __csrc(CSR_MIEH, 1 << (iid >> 1));

    return 0;
}


/// @name   m_ext_enable()
/// @brief  Enable/disable IMSIC interrupt by EIID 
int m_ext_enable(int eiid, int enable) {

    register unsigned long ireg = 0xC0 + (eiid >> 5);
    register unsigned long ival = 1 << (eiid % 32);

    __csrw(CSR_MISELECT, ireg);

    if (enable)
        __csrs(CSR_MIREG, ival);
    else    
        __csrc(CSR_MIREG, ival);

    return 0;
}


/// @name   m_ext_delivery(enable)
/// @brief  Enable/disable IMSIC MSI delivery
int m_ext_delivery(int enable) {

    __csrw(CSR_MISELECT, 0x70);

    if (enable)
        __csrw(CSR_MIREG, 1);
    else    
        __csrw(CSR_MIREG, 0);

    return 0;
}


/// @name   m_ext_threshold(threshold)
/// @brief  Set IMSIC priority threshold level
int m_ext_threshold(int threshold) {

    __csrw(CSR_MISELECT, 0x72);
    __csrw(CSR_MIREG, (unsigned long)threshold);

    return 0;    
}


/// @name   m_all_enable( enable )
/// @brief  Enable/disable M-mode interrupts (mstatus.MIE)
int m_all_enable(int enable) {

    if (enable) 
        __csrs(CSR_MSTATUS, 1 << 3);
    else    
        __csrc(CSR_MSTATUS, 1 << 3);

    return enable;
}
