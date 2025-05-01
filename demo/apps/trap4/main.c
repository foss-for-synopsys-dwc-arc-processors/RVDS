/// @file   main.c
/// @brief  RISC-V Demo Application  - M-mode interrupt traps, nested vectored mode

#include "arch.h"
#include "mtvec.h"
#include "tmon.h"


// M-mode interrupt callbacks

static void m_msi_callback(void ) {

        register unsigned long mip, mie;

        TRACE("M-mode MSWI callback\n");

        __csrr(mip, CSR_MIP);
        __csrr(mie, CSR_MIE);
        TRACE("mip=0x%lx; mie=0x%lx\n", mip, mie);

        tmon_call(TMON_FID_MSWI, 0);    // de-assert M-mode SWI
}


static void m_ssi_callback(void ) {

        register unsigned long mip, mie;

        TRACE("M-mode SSWI callback\n");

        __csrr(mip, CSR_MIP);
        __csrr(mie, CSR_MIE);
        TRACE("mip=0x%lx; mie=0x%lx\n", mip, mie);

        tmon_call(TMON_FID_SSWI, 0);    // de-assert S-mode SWI
}


// static void m_ssi_callback_1(void ) {
// 
//         register unsigned long mip, mie;
// 
//         TRACE("M-mode SSWI callback\n");
// 
//         __csrr(mip, CSR_MIP);
//         __csrr(mie, CSR_MIE);
//         TRACE("mip=0x%lx; mie=0x%lx\n", mip, mie);
// }


static void m_ext_callback(void ) {

        TRACE("M-mode external interrupt callback\n");
}


static unsigned long mssi[]  = { 32+1,  (unsigned long)m_ssi_callback };
static unsigned long mmsi[]  = { 32+20, (unsigned long)m_msi_callback };
static unsigned long mext[]  = { 32+10, (unsigned long)m_ext_callback };
static unsigned long mssi1[] = { 32+1,  0 };


int main(void)
{

        unsigned long mip, mie;


        // printf("%s:%d RISC-V  Demo App - Privilege Mode Transitions\n", __FILE__, __LINE__);
        TRACE("RISC-V Demo App - M-mode interrupt traps, nested vectored mode\n");

        /* M-mode setup */

        m_trap_mode(TRAP_MODE_NESTED);
        
        // m_maj_setvec(TRAP_IID_MSWI, m_swi_handler);
        // m_maj_setvec(TRAP_IID_SSWI, s_swi_handler);
        // m_ext_setvec(10, m_ext_handler);        

        m_maj_priority(TRAP_IID_SSWI, 1);
        m_maj_priority(TRAP_IID_MSWI, 20);

        tmon_call(TMON_FID_CB, mmsi);
        tmon_call(TMON_FID_CB, mssi);
        tmon_call(TMON_FID_CB, mext);

        m_maj_enable(TRAP_IID_MSWI, 1);
        m_maj_enable(TRAP_IID_SSWI, 1);
        m_maj_enable(TRAP_IID_MEXT, 1);

        m_ext_enable(1, 1);             // enable ext #1  (SSI->IMSIC) 
        m_ext_enable(20, 1);            // enable ext #2  (MSI->IMSIC) 
        m_ext_enable(10, 1);            // enable ext #10 (ext->IMSIC)                    

        m_ext_delivery(1);

        /* Test needs M-mode privileges */

    CASE(1);        

        m_all_enable(0);                // disable M-mode interrupts

        tmon_call(TMON_FID_EXPECT, 32+1);
        tmon_call(TMON_FID_EXPECT, 32+20);

        tmon_call(TMON_FID_SSWI, 1);    // assert S-mode SWI
        tmon_call(TMON_FID_MSWI, 1);    // assert M-mode SWI


        __csrr(mip, CSR_MIP);
        __csrr(mie, CSR_MIE);
        TRACE("mip=0x%lx; mie=0x%lx\n", mip, mie);


        m_all_enable(1);                // enable M-mode interrupts
        
        tmon_call(TMON_FID_VERIFY, 0);

    CASE(2);

        m_all_enable(0);

        tmon_call(TMON_FID_EXPECT, 32+10);
        tmon_call(TMON_FID_MMSI, 10)    // asser MSI #10

        tmon_call(TMON_FID_MMSI, 10);   // assert M-mode external interrupt (in IMSIC)

        m_all_enable(1);

        tmon_call(TMON_FID_VERIFY, 0);

    CASE(3);

        m_all_enable(0);

        tmon_call(TMON_FID_EXPECT, 32+1);
        tmon_call(TMON_FID_EXPECT, 32+10);
        tmon_call(TMON_FID_EXPECT, 32+20);

        tmon_call(TMON_FID_SSWI, 1);    // assert S-mode SWI
        tmon_call(TMON_FID_MSWI, 1);    // assert M-mode SWI
        tmon_call(TMON_FID_MMSI, 10);   // assert M-mode external interrupt (in IMSIC)

        m_all_enable(1);

        tmon_call(TMON_FID_VERIFY, 0);

    /* M-mode SSI, mip.SSI remains set (SSI is not de-asserted) */
    CASE(4);

        m_all_enable(0);                // disable M-mode interrupts

        // M-mode SSI callback, do not clear SSI
        tmon_call(TMON_FID_CB, mssi1);     

        tmon_call(TMON_FID_EXPECT, 32+1);
        tmon_call(TMON_FID_EXPECT, 32+1);

        tmon_call(TMON_FID_SSWI, 1);    // assert S-mode SWI

        __csrr(mip, CSR_MIP);
        __csrr(mie, CSR_MIE);

        TRACE("mip=0x%lx; mie=0x%lx\n", mip, mie);

        m_all_enable(1);                // enable M-mode interrupts

        tmon_call(TMON_FID_SSWI, 1);    // assert S-mode SWI

        __csrr(mip, CSR_MIP);
        __csrr(mie, CSR_MIE);

        TRACE("mip=0x%lx; mie=0x%lx\n", mip, mie);

        tmon_call(TMON_FID_VERIFY, 0);

        exit(0);
}
