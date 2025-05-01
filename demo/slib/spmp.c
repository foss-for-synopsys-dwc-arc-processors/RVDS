/// @file   spmp.c
/// @brief  RISC-V Virtual Platform - RISC-V S-mode PMP test driver implementation

#include "arch.h"
#include "spmp.h"


/// @name   ret_t spmp_set_entry (spmp_cfg_t *config, unsigned long index, unsigned long addr, unsigned long attr)
/// @brief  set SPMP entry (address and attributes) in the shadow configuration
ret_t spmp_set_entry (spmp_cfg_t *config, unsigned long index, unsigned long addr, unsigned long attr) {

    v4u8_t *pattr = (v4u8_t *)(&(config->attr[index / 4]));

    pattr->u8[index % 4] = attr; 
    config->addr[index]  = addr >> 2;

    return (ret_t){ 0, 0 };
}


/// @name   ret_t spmp_set_switch (spmp_cfg_t *config, unsigned long switch0, unsigned long switch1 )
/// @brief  set SPMP switch mask in the shadow configuration 
ret_t spmp_set_switch (spmp_cfg_t *config, unsigned long switch0, unsigned long switch1 ) {

    config->mask = (v2u32_t){{switch0, switch1}};

    return (ret_t){ 0, 0 };
}


/// @name   ret_t spmp_config_apply(spmp_cfg_t *config)
/// @brief  load SPMP configuration form memory buffer to the CSRs
ret_t spmp_config_apply(spmp_cfg_t *config) {

    spmp_switch( (v2u32_t){{ 0, 0 }} );

    for (int i=0; i<64; i++) {
        __csr_write(CSR_SPMPADDR + i, config->addr[i]);
    }
    for (int i=0; i<16; i++) {
        __csr_write(CSR_SPMPCFG  + i, config->attr[i]);
    }

    spmp_switch( config->mask );

    return (ret_t){0, 0};
}


/// @name   ret_t spmp_config_dump(spmp_cfg_t *config)
/// @brief  dump actual SPMP configuration from CSRs
ret_t spmp_config_dump(spmp_cfg_t *config) {

    for (int i=0; i<64; i++) {
        config->addr[i] = __csr_read(CSR_SPMPADDR + i) << 2;
    }

    for (int i=0; i<16; i++) {
        config->attr[i] = __csr_read(CSR_SPMPCFG  + i);
    }

    __csrr(config->mask.u32[0], spmpswitch0);
    __csrr(config->mask.u32[1], spmpswitch1);


    return (ret_t){0, 0};

}

/// @name   ret_t spmp_config_show (spmp_cfg_t *config)
/// @brief  display active SPMP configuration (non-zero entries)
/// @note   this is shadow configuration copy, dump CSRs prior 
///         call to see the actual configuration.
ret_t spmp_config_show (spmp_cfg_t *config) {

    v4u8_t *pattr;

    printf("%s: actual SPMP configuration (zero entries are hidden)\n", __func__);

    for (int i=0; i<64; i++) {

        // Display non-zero SPMP entries 
        if (config->addr[i] != 0) {
            pattr = (v4u8_t *)(&(config->attr[i / 4]));
            // attention: actual spmpaddr{i} value is >> 2 
            printf("  spmpaddr%d: %lx; spmpcfg%d[%d]: %x;\n", i, config->addr[i], i/4, i%4, pattr->u8[i % 4]);
        }
    }

    printf("  spmpswitch0: %lx;\n", config->mask.u32[0]);
    printf("  spmpswitch1: %lx;\n", config->mask.u32[1]);

    return (ret_t){0, 0};
}


/// @name   ret_t spmp_switch(u64_t mask)
/// @brief  apply mask to SPMP switch registers 
ret_t spmp_switch(v2u32_t mask) {

    __csrw(spmpswitch0, mask.u32[0]);
    __csrw(spmpswitch1, mask.u32[1]);

    return (ret_t){0, 0};

}



