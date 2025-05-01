/// @file   main.c
/// @brief  RISC-V Test Monitor - privilege mode transitions (M/S/VS-mode ecall traps)

#include "arch.h"
#include "mtvec.h"
#include "stvec.h"
#include "tmon.h"


int main(void)
{

        TRACE("RISC-V VP Demo App - Privilege Mode Transitions, M/S/VS-mode environment calls\n");

        /* M-mode setup */

        m_trap_mode(TRAP_MODE_DIRECT);

        m_exc_delegate(TRAP_EID_UCALL, 1);
        m_exc_delegate(TRAP_EID_VCALL, 1);

        /* S-mode setup */

        tmon_call(TMON_FID_PRIV, S_MODE);

        s_trap_mode(TRAP_MODE_DIRECT);

        /* Test needs S-mode privileges */

        /* CASE#1: */
        CASE(1);        
        tmon_call(TMON_FID_PRIV, S_MODE);             // to S


        /* CASE#2: */
        CASE(2);        
        tmon_call(TMON_FID_PRIV, U_MODE);            // to U


        /* CASE#3:  */
        CASE(3);        
        tmon_call(TMON_FID_PRIV, M_MODE);            // to M


        /* CASE#4:  */
        CASE(4);        
        tmon_call(TMON_FID_PRIV, VM_MODE);           // to M


        /* CASE#5: */
        CASE(5);        
        tmon_call(TMON_FID_PRIV, VU_MODE);           // to VU


        /* CASE#6: */
        CASE(6);        
        tmon_call(TMON_FID_PRIV, VS_MODE);           // to VS


        exit(0);
}
