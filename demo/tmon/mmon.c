/// @file   mmon.c
/// @brief  RISC-V Test Monitor - M-mode Monitor Services

#include "tmon.h"

typedef void (*tmon_ecall_t)(void *s);


static void mmon_fail  (void *s);
static void mmon_exit  (void *s);
static void mmon_csrr  (void *s);
static void mmon_csrw  (void *s);
static void mmon_csrs  (void *s);
static void mmon_csrc  (void *s);
static void mmon_icsrr (void *s);
static void mmon_icsrw (void *s);
static void mmon_icsrs (void *s);
static void mmon_icsrc (void *s);
static void mmon_priv  (void *s);
static void mmon_expect(void *s);
static void mmon_verify(void *s);
static void mmon_mswi  (void *s);
static void mmon_sswi  (void *s);
static void mmon_mmsi  (void *s);
static void mmon_smsi  (void *s);
static void mmon_cb    (void *s);


static tmon_ecall_t m_fid_vector[] = {
    mmon_fail,          // FID=0  - TMON_FID_FAIL
    mmon_exit,          // FID=1  - TMON_FID_EXIT 
    mmon_csrr,          // FID=2  - TMON_FID_CSRR
    mmon_csrw,          // FID=3  - TMON_FID_CSRW
    mmon_csrs,          // FID=4  - TMON_FID_CSRS
    mmon_csrc,          // FID=5  - TMON_FID_CSRC
    mmon_icsrr,         // FID=6  - TMON_FID_ICSRR 
    mmon_icsrw,         // FID=7  - TMON_FID_ICSRW
    mmon_icsrs,         // FID=8  - TMON_FID_ICSRS 
    mmon_icsrc,         // FID=9  - TMON_FID_ICSRC
    mmon_priv,          // FID=10 - TMON_FID_PRIV
    mmon_expect,        // FID=11 - TMON_FID_EXPECT
    mmon_verify,        // FID=12 - TMON_FID_VERIFY
    mmon_mswi,          // FID=13 - TMON_FID_MSWI
    mmon_sswi,          // FID=14 - TMON_FID_SSWI
    mmon_mmsi,          // FID=15 - TMON_FID_MMSI
    mmon_smsi,          // FID=16 - TMON_FID_SMSI
    mmon_cb,            // FID=17 - TMON_FID_CB
};


/// @name  void m_exc_ecall( *s )
/// @brief M-mode environment calls entry point
void m_exc_ecall(void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    fid_t fid = (fid_t)(sf[4]);

    m_fid_vector[fid](s);

    sf[17] += 4;        // move EPC to the next (after ecall) instruction   

    return;
}


/// @name   mmon_fail( *s )
/// @brief
static void mmon_fail(void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    ERROR("Fatal Error\n");
    TRACE("pc: 0x%lx; ra: 0x%lx; status: %lx; error: %ld;\n", sf[17], sf[0], sf[16], sf[4] );

    exit(-1);
} 

/// @name mmon_exit( *s )
/// @brief
static void mmon_exit (void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    exit(sf[5]);        // exit(a1)     
}

/// @name   mmon_csrr( *s ) 
/// @brief
static void mmon_csrr (void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    WARNING("not implemented\n");
    sf[4] = 0;          // a0 = OK
}


/// @name   mmon_csrw ( *s )
/// @brief
static void mmon_csrw (void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    WARNING("not implemented\n");
    sf[4] = 0;          // a0 = OK
}


/// @name   mmon_csrs( *s )
/// @brief
static void mmon_csrs (void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    WARNING("not implemented\n");
    sf[4] = 0;          // a0 = OK
}


/// @name   mmon_csrc( *s )
/// @brief
static void mmon_csrc (void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    WARNING("not implemented\n");
    sf[4] = 0;          // a0 = OK
}


/// @name   mmon_icsrr( *s )
/// @brief
static void mmon_icsrr(void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    WARNING("not implemented\n");
    sf[4] = 0;          // a0 = OK
}


/// @name   mmon_icsrw( *s )
/// @brief
static void mmon_icsrw(void *s) {

    register unsigned long *sf = (unsigned long *)s;

    WARNING("not implemented\n");
    sf[4] = 0;          // a0 = OK
}


/// @name   mmon_icsrs( *s )
/// @brief
static void mmon_icsrs(void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    WARNING("not implemented\n");
    sf[4] = 0;          // a0 = OK
}


/// @name   mmon_icsrc( *s )
/// @brief
static void mmon_icsrc(void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    WARNING("not implemented\n");
    sf[4] = 0;          // a0 = OK
}


/// @name   mmon_priv ( *s )
/// @brief  M-mode version of privilege switch FID handler
static void mmon_priv (void *s ) {

    register unsigned long *sf = (unsigned long *)s;

    unsigned long status  = sf[16];                 // mstatus 
    unsigned long statush = sf[19];                 // mstatush
    unsigned long next    = (unsigned long)(sf[5]); // a1: pointer to next privilege & virtualization mode (requested by environment call) 

    unsigned long mpp     = ((status >> 11) & 0x3); // machine previous privilege mode (equal to mstatus.MPP)
    unsigned long mpv     = ((statush >> 7) & 0x1); // machine previous virtualization mode (equal to mstatush.MPV)
    unsigned long mode    = ((mpv << 2) | mpp);

    switch ( next ) {
        case 0x00:
        case 0x01:
        case 0x03:
        case 0x04:
        case 0x05:
            TRACE("%s to %s privilege mode switch\n", priv_s[mode], priv_s[next]);
            break;
        default:
            ERROR("target privilege mode specification {%ld} is invalid\n", next);
            exit(-1);
    }

    // update privilege mode stack in status/statush values 
    sf[16] = (status & ~(3 << 11)) | ((next & 0x03) << 11);
    sf[19] = (statush & ~(1 << 7)) | ((next >> 2) << 7);

    sf[4] = 0;           // OK in a0

    return;
}


/// @name   mmon_expect( *s )
/// @brief  set expectation of specified trap 
/// @param  
static void mmon_expect(void *s) {
    
    register unsigned long *sf      = (unsigned long *)s;
    register unsigned long trap_id  = sf[5];

    TRACE("expect trap #%ld\n", trap_id);

    queue_append(trap_id);

    sf[4] = 0;      // OK
    
    return;
}


/// @name   mmon_verify( *s )
/// @brief  verify that expected trap had been raised
/// @param
static void mmon_verify(void *s ) {

    register unsigned long *sf      = (unsigned long *)s;

    if ( 0 != queue_is_empty() ) {
        ERROR("expected trap queue is not empty, \n");
        exit(-1);
    }

    sf[4] = 0;      // OK
    
    return;
}


/// @name   mmon_mswi( *s )
/// @brief  assert/de-assert m-mode SWI
/// @param
static void mmon_mswi  (void *s) {


    register unsigned long *sf      = (unsigned long *)s;
    register unsigned long enable   = sf[5];    

    __mmio_base[0] = (enable) ? 1 : 0;

}

/// @name   mmon_sswi( *s )
/// @brief  assert/de-assert s-mode SWI
/// @param
static void mmon_sswi  (void *s) {

    register unsigned long *sf      = (unsigned long *)s;
    register unsigned long enable   = sf[5];    

    __mmio_base[0xC000/4] = (enable) ? 1 : 0;

}


/// @name   mmon_mmsi( *s )
/// @brief  send M-mode MSI interrupt
/// @param
static void mmon_mmsi  (void *s) {

    register unsigned long *sf    = (unsigned long *)s;
    register unsigned long eiid   = sf[5];      // in a1

    __mmsi_base[0] = eiid;

}


/// @name   mmon_smsi( *s )
/// @brief  send S-mode MSI interrupt
/// @param
static void mmon_smsi  (void *s) {

    register unsigned long *sf    = (unsigned long *)s;
    register unsigned long eiid   = sf[5];      // in a1

    __smsi_base[0] = eiid;

}


/// @name   mmon_cb( *s )
/// @brief  link/unlink user callback to the specified tmon trap handler
/// @param
static void mmon_cb  (void *s) {

    register unsigned long *sf    = (unsigned long *)s;
    register unsigned long *cb    = (unsigned long *)sf[5];      // in a1

    if (0 != cb[1]) {
        TRACE("link user callback @0x%lx to M-mode trap #%ld\n", cb[1], cb[0]);
        m_trap_callback[cb[0]] = (void*)cb[1];
    }
    else {
        TRACE("unlink user callback from M-mode trap #%ld\n", cb[0]);
        m_trap_callback[cb[0]] = (void*)0;
    }


}
