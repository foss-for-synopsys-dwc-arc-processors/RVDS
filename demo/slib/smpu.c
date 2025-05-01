/// @file   smpu.c 
/// @brief  RISC-V Shared Library - S-mode MPU test driver code

#include "arch.h"
#include "smpu.h"


/// @name   {x}mpu_region_config( id, entry)
/// @brief  load single SMPU entry configuration to iCSRs
ret_t smpu_region_config (int id, const unsigned long *entry) {
    
    __icsrw( ICSR_SMPU_BASE + (id << 1) + 0, entry[0] );
    __icsrw( ICSR_SMPU_BASE + (id << 1) + 1, entry[1] );

    return (ret_t){ 0, 0 };
}

ret_t hmpu_region_config (int id, const unsigned long *entry) {
    
    __icsrw( ICSR_HMPU_BASE + (id << 1) + 0, entry[0] );
    __icsrw( ICSR_HMPU_BASE + (id << 1) + 1, entry[1] );

    return (ret_t){ 0, 0 };
}



/// @name   {x}mpu_region_enable ( id )
/// @brief  enable SMPU region by number
ret_t smpu_region_enable ( int id ) {
    
    __csrs( CSR_SMPUMASK, 1 << id );

    return (ret_t){ 0, 0 };
}

ret_t hmpu_region_enable ( int id ) {
    
    __csrs( CSR_HMPUMASK, 1 << id );

    return (ret_t){ 0, 0 };
}


/// @name   {x}mpu_region_disable( id )
/// @brief  disable SMPU region by number
ret_t smpu_region_disable( int id ) {

    __csrc( CSR_SMPUMASK, 1 << id );

    return (ret_t){ 0, 0 };
}

ret_t hmpu_region_disable( int id ) {

    __csrc( CSR_HMPUMASK, 1 << id );

    return (ret_t){ 0, 0 };
}


/// @name   {x}mpu_switch ( mask )
/// @brief  apply SMPU mask 
ret_t smpu_switch        ( unsigned long mask ) {

    __csrw( CSR_SMPUMASK, mask );

    return (ret_t){ 0, 0 };
}

ret_t hmpu_switch        ( unsigned long mask ) {

    __csrw( CSR_HMPUMASK, mask );

    return (ret_t){ 0, 0 };
}


/// @name   {x}mpu_group_config(id, num, entry)
/// @brief  load configuration to the specified number of SMPU entries to iCSRs
ret_t smpu_group_config(int id, int num, const unsigned long entries[][2]) {

    for (int i = 0; i < num; i++) {
        __icsrw( ICSR_SMPU_BASE + (id << 1) + 0, entries[i][0] );
        __icsrw( ICSR_SMPU_BASE + (id << 1) + 1, entries[i][1] );
        id++;
    }

    return (ret_t){ 0, 0 };
}

ret_t hmpu_group_config(int id, int num, const unsigned long entries[][2]) {

    for (int i = 0; i < num; i++) {
        __icsrw( ICSR_HMPU_BASE + (id << 1) + 0, entries[i][0] );
        __icsrw( ICSR_HMPU_BASE + (id << 1) + 1, entries[i][1] );
        id++;
    }

    return (ret_t){ 0, 0 };
}


/// @name   {x}mpu_group_enable ( mask )
/// @brief  enable/disable regions by mask
ret_t smpu_group_enable  ( int enable, unsigned long mask ) {
    
    if ( enable ) {
        __csrs( CSR_SMPUMASK, mask );
    }
    else {
        __csrc( CSR_SMPUMASK, mask );
    }

    return (ret_t){ 0, 0 };
}


ret_t hmpu_group_enable  ( int enable, unsigned long mask ) {
    
    if ( enable ) {
        __csrs( CSR_HMPUMASK, mask );
    }
    else {
        __csrc( CSR_HMPUMASK, mask );
    }

    return (ret_t){ 0, 0 };
}


/// @name   {x}mpu_disable()
/// @brief  disable SMPU
ret_t smpu_disable( void ) {

    __csrw(CSR_SMPUMASK, 0);    

    return (ret_t){ 0, 0 };
}

ret_t hmpu_disable( void ) {

    __csrw(CSR_HMPUMASK, 0);    

    return (ret_t){ 0, 0 };
}


/// @name   {x}mpu_config_show( mask )
/// @brief  display SMPU configuration, show acrive regions by mask   
ret_t smpu_config_show( unsigned long mask ) {

    unsigned long addr, conf, psz, enabled;

    printf("%s: dumping SMPU L1 (S-mode) configuration, inactive regions are hidden\n", __func__);

    __csrr(enabled, CSR_SMPUMASK);
    printf("  smpumask: 0x%lx\n", enabled);

    for (int id = 0; id < 32; id++ ) {

        __icsrr( addr, ICSR_SMPU_BASE + (id << 1) + 0);
        __icsrr( conf, ICSR_SMPU_BASE + (id << 1) + 1);

        psz = addr & 0x1f;  // 0 in case of protected region 

        if (( conf != 0 ) || ( addr != 0 )) {

            printf("  region#%d: %s ", id, ( enabled & (1 << id) )  ? "on " : "off" );

            if ( psz )  {
                // translated region
                printf("0x%lx->0x%lx %dK 0x%lx\n", addr & ~0x1f, conf & ~0x1f, 1 << (psz - 10),  conf & 0x0ff );
            }
            else {
                // protected region
                printf("0x%lx-0x%lx 0x%lx\n", addr, addr + (conf | 0x1F), conf & 0x1F );
            }
        }
    }

    return (ret_t){ 0, 0 };
}

ret_t hmpu_config_show( unsigned long mask ) {

    unsigned long addr, conf, psz, enabled;

    printf("%s: dumping SMPU L2 (HS-mode) configuration, inactive regions are hidden\n", __func__);

    __csrr(enabled, CSR_HMPUMASK);
    printf("  hmpumask: 0x%lx\n", enabled);

    for (int id = 0; id < 32; id++ ) {

        __icsrr( addr, ICSR_HMPU_BASE + (id << 1) + 0);
        __icsrr( conf, ICSR_HMPU_BASE + (id << 1) + 1);

        psz = addr & 0x1f;  // 0 in case of protected region 

        if (( conf != 0 ) || ( addr != 0 )) {

            printf("  region#%d: %s ", id, ( enabled & (1 << id) )  ? "on " : "off" );

            if ( psz )  {
                // translated region
                printf("0x%lx->0x%lx %dK 0x%lx\n", addr & ~0x1f, conf & ~0x1f, 1 << (psz - 10),  conf & 0x0ff );
            }
            else {
                // protected region
                printf("0x%lx-0x%lx 0x%lx\n", addr, addr + (conf | 0x1F), conf & 0x1F );
            }
        }
    }

    return (ret_t){ 0, 0 };
}

