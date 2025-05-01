/// @file   smpu.h
/// @brief  RISC-V Shared Library - MPU L1/L2 (S/HS-mode) test driver header file


#pragma once

#include "tmon.h"

/* 
    S-mode MPU Memory Attributes:
     
    SMPU_ATTR_Sxx   - Shared S/U-mode `xx` attributes 
    SMPU_ATTR_Uxx   - U-mode only `xx` attributes
    SMPU_ATTR_xx    - S-mode only `xx`  attributes
*/

typedef enum smpu_attr_e {
    /* Shared S/U-mode access */
    SMPU_ATTR_SRO       = 0x02,     // r-- r-- S/U-mode read-only
    SMPU_ATTR_SXR       = 0x03,     // r-x --x S/U-mode execute, S-mode read
    SMPU_ATTR_SRW       = 0x0A,     // rw- rw- S/U-mode read-write
    SMPU_ATTR_SRX       = 0x0B,     // r-- r-x S/U-mode read, U-mode execute
    /* S-mode access .U = 0 */
    SMPU_ATTR_XO        = 0x01,     // S-mode fetch only
    SMPU_ATTR_RO        = 0x04,     // S-mode read only
    SMPU_ATTR_RX        = 0x05,     // S-mode read/execute
    SMPU_ATTR_RW        = 0x06,     // S-mode read/write
    SMPU_ATTR_RWX       = 0x07,     // S-mode read/write/fetch
    /* U-mode access .U = 1 */
    SMPU_ATTR_UXO       = 0x09,     // U-mode fetch only
    SMPU_ATTR_URO       = 0x0c,     // U-mode read only
    SMPU_ATTR_URX       = 0x0d,     // U-mode read/execute
    SMPU_ATTR_URW       = 0x0e,     // U-mode read/write
    SMPU_ATTR_URWX      = 0x0f,     // U-mode read/write/fetch
    SMPU_ATTR_
} smpu_attr_t;


/* MPU L2 (HS-mode) memory access permissions */

typedef enum hmpu_attr_e {
    /* VS-mode access .U = 0 */
    HMPU_ATTR_XO        = 0x01,
    HMPU_ATTR_RO        = 0x04,
    HMPU_ATTR_RX        = 0x05,
    HMPU_ATTR_RW        = 0x06,
    HMPU_ATTR_RWX       = 0x07,        
    /* VS/VU-mode access .U = 1 */    
    HMPU_ATTR_SXO       = 0x09,
    HMPU_ATTR_SRO       = 0x0c,
    HMPU_ATTR_SRX       = 0x0d,
    HMPU_ATTR_SRW       = 0x0e,
    HMPU_ATTR_SRWX      = 0x0f,        
} hmpu_attr_t;


/* MPU L1/L2 (S/HS-mode) single region operations */

ret_t smpu_region_config ( int id, const unsigned long *entry );
ret_t smpu_region_enable ( int id );
ret_t smpu_region_disable( int id );

ret_t hmpu_region_config ( int id, const unsigned long *entry );
ret_t hmpu_region_enable ( int id );
ret_t hmpu_region_disable( int id );


/* MPU L1/L2 (S/HS-mode) multi regions (group) operations */

ret_t smpu_group_config  ( int start_id, int num, const unsigned long entries[][2] );
ret_t smpu_group_enable  ( int enable, unsigned long mask );

ret_t hmpu_group_config  ( int start_id, int num, const unsigned long entries[][2] );
ret_t hmpu_group_enable  ( int enable, unsigned long mask );

/* MPU L1/L2 (S/HS-mode) regions mask control */

ret_t smpu_disable       ( void );
ret_t smpu_switch        ( unsigned long mask );

ret_t hmpu_disable       ( void );
ret_t hmpu_switch        ( unsigned long mask );

/* test helper functions */

ret_t smpu_config_show( unsigned long mask );
ret_t hmpu_config_show( unsigned long mask );

