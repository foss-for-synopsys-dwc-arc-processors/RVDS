/// @file   main.c
/// @brief  RISC-V Demo Application  - M-mode interrupt traps, direct mode

#include "arch.h"
#include "mtvec.h"
#include "tmon.h"


// dummy M-mode maj interrupt handlers (replaces default)

static void m_msi_callback(void ) {

        TRACE("M-mode MSI callback\n");
        tmon_call(TMON_FID_MSWI, 0);    // de-assert M-mode SWI
}

static void m_ssi_callback(void ) {

        TRACE("M-mode SSI callback\n");
        tmon_call(TMON_FID_SSWI, 0);    // de-assert S-mode SWI
}

static void m_ext_callback(void ) {

        TRACE("M-mode external interrupt callback\n");
}

static unsigned long msi_cb[] = { 32+TRAP_IID_MSWI, (unsigned long)m_msi_callback };
static unsigned long ssi_cb[] = { 32+TRAP_IID_SSWI, (unsigned long)m_ssi_callback };
static unsigned long ext_cb[] = { 64+10,            (unsigned long)m_ext_callback };

int main(void)
{

        // printf("%s:%d RISC-V  Demo App - Privilege Mode Transitions\n", __FILE__, __LINE__);
        TRACE("RISC-V Demo App - M-mode interrupt traps, direct mode\n");

        /* M-mode setup */

        m_trap_mode(TRAP_MODE_DIRECT);
        
        m_maj_enable(TRAP_IID_MSWI, 1);
        m_maj_enable(TRAP_IID_SSWI, 1);

        
        // m_maj_setvec(TRAP_IID_MSWI, m_msi_callback);
        // m_maj_setvec(TRAP_IID_SSWI, s_ssi_callback);

        tmon_call(TMON_FID_CB, &ssi_cb);
        tmon_call(TMON_FID_CB, &msi_cb);

        // m_ext_setvec(10, m_ext_callback);
        // tmon_call(TMON_FID_CB, &ext_cb);

        /* Test needs M-mode privileges */
    CASE(1);        

        m_all_enable(0);                                // disable M-mode interrupts

        m_maj_priority(TRAP_IID_SSWI, 1);               // set MSWI prio
        m_maj_priority(TRAP_IID_MSWI, 2);               // set SSWI prio

        tmon_call(TMON_FID_MSWI, 1);                    // assert MSWI
        tmon_call(TMON_FID_SSWI, 1);                    // assert SSWI

        tmon_call(TMON_FID_EXPECT, 32+TRAP_IID_SSWI);   // expect SWSI
        tmon_call(TMON_FID_EXPECT, 32+TRAP_IID_MSWI);   // expect MWSI

        m_all_enable(1);                                // enable M-mode interrupts

        tmon_call(TMON_FID_VERIFY, 0);
        
    CASE(2);

        m_all_enable(0);                                // disable M-mode interrupts

        m_maj_enable(TRAP_IID_MEXT, 1);                 // enable maj external

        m_ext_enable(10, 1);                            // enable external #10
        m_ext_delivery(1);                              // enable external delivery

        tmon_call(TMON_FID_MMSI, 10);                   // assert M-mode external interrupt #10 (in IMSIC)

        tmon_call(TMON_FID_EXPECT, 64+10);              // expect external #10

        m_all_enable(1);                                // enable M-mode interrupts

        exit(0);
}
