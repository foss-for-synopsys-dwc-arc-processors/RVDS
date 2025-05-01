/// @file   main.c
/// @brief  RISC-V Virtual Platform - Machine interrupt traps, direct mode

#include "arch.h"
#include "mtvec.h"
#include "tmon.h"


// dummy M-mode maj interrupt handlers (replaces default)

extern void m_tmr_handler(void ) {}
extern void m_swi_handler(void ) {}

int main(void)
{

        // printf("%s:%d RISC-V VP Demo App - Privilege Mode Transitions\n", __FILE__, __LINE__);
        TRACE("RISC-V VP Demo App - Privilege Mode Transitions\n");

        /* M-mode setup */

        m_trap_mode(TRAP_MODE_DIRECT);
        
        m_exc_delegate(TRAP_EID_UCALL, 0);
        m_exc_delegate(TRAP_EID_VCALL, 0);


        m_maj_setvec(TRAP_IID_MTMR, m_tmr_handler);
        m_maj_setvec(TRAP_IID_MSWI, m_swi_handler);
  
        /* S-mode setup */
        
        tmon_call(TMON_FID_PRIV, S_MODE);


//        s_trap_mode(TRAP_MODE_DIRECT);
//
//        s_exc_delegate(VECALL_EID, 1);
//
//        s_maj_setvec(SUPERVISOR_TMR_IID, s_tmr_handler);
//        s_maj_setvec(SUPERVISOR_SWI_IID, s_swi_handler)


        
        /* VS-mode setup */

        tmon_call(TMON_FID_PRIV, VS_MODE);

//        s_trap_mode(TRAP_MODE_DIRECT);
//
//        s_maj_setvec(SUPERVISOR_TMR_IID, v_tmr_handler);
//        s_maj_setvec(SUPERVISOR_SWI_IID, v_swi_handler)



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
