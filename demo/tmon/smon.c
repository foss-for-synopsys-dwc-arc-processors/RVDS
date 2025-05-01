/// @file   mmon.c
/// @brief  RISC-V Test Monitor - M-mode Monitor Services

#include "tmon.h"

typedef void (*smon_fid_t)(void *s);

static void smon_fail(void *s);
static void smon_exit(void *s);
static void smon_priv(void *s);
static void smon_forward(void *s);

static smon_fid_t s_fid_vector[] = {
    smon_fail,          // FID=0  - TMON_FID_FAIL
    smon_exit,          // FID=1  - TMON_FID_EXIT 
    smon_forward,       // FID=2  - TMON_FID_CSRR
    smon_forward,       // FID=3  - TMON_FID_CSRW
    smon_forward,       // FID=4  - TMON_FID_CSRS
    smon_forward,       // FID=5  - TMON_FID_CSRC
    smon_forward,       // FID=6  - TMON_FID_ICSRR 
    smon_forward,       // FID=7  - TMON_FID_ICSRW
    smon_forward,       // FID=8  - TMON_FID_ICSRS 
    smon_forward,       // FID=9  - TMON_FID_ICSRC
    smon_priv,          // FID=10 - TMON_FID_PRIV
    smon_forward,       // FID=11 - TMON_FID_EXPECT
    smon_forward,       // FID=12 - TMON_FID_VERIFY
    smon_forward,       // FID=13 - TMON_FID_MSWI
    smon_forward,       // FID=14 - TMON_FID_SSWI
    smon_forward,       // FID=15 - TMON_FID_MMSI
    smon_forward,       // FID=16 - TMON_FID_SMSI
};


/// @name  void s_exc_ecall( *s )
/// @brief test monitor S-mode environment call exception trap entry point
void s_exc_ecall(void *s ) {

    register unsigned long *sf = (unsigned long *)s;
    fid_t fid = (fid_t)(sf[4]);
        
    s_fid_vector[fid](s);

    ERROR("Unsupported S-mode ecall exception trap.\n");
    TRACE("pc: 0x%lx; status: %lx;\n", sf[17], sf[16] );
    exit(-1);

    sf[17] += 4;        // move EPC to the next (after ecall) instruction   

    return;
}



/// @name
/// @brief
static void smon_fail(void *s) {

    register unsigned long *sf = (unsigned long *)s;

    ERROR("Fatal error\n");
    TRACE("pc: 0x%lx; ra: 0x%lx; status: %lx; error: %ld;\n", sf[17], sf[0], sf[16], sf[4] );

    exit(-1);

}


/// @name
/// @brief
static void smon_exit(void *s) {

    register unsigned long *sf = (unsigned long *)s;

    exit(sf[5]);        // exit(a1)
}


/// @name
/// @brief
static void smon_priv(void *s) {

    register unsigned long *sf = (unsigned long *)s;

    // not yet implemented
    ERROR("switching privilege from delegated S-mode trap is not implemented yet\n");
    exit(-1);

}


/// @name
/// @brief
static void smon_forward(void *s) {

    register unsigned long *sf = (unsigned long *)s;

    TRACE("S-mode forwards ecall(0x%lx, 0x%lx) to M-mode\n", sf[4], sf[5]);

    tmon_call(sf[4], sf[5]); 
}
