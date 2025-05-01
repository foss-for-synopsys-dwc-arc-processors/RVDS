/// @file   spmp.h
/// @brief  RISC-V Virtual Platform - RISC-V S-mode PMP header file

#pragma once

#include "tmon.h"

// typedef struct {
//     unsigned long   lo;
//     unsigned long   hi;
// } u64_t;
// 
// 
// typedef union {
//     unsigned long u32;
//     unsigned char u8[4];
// } v4u8_t;


typedef struct spmp_cfg_s {
    v2u32_t         mask;
    unsigned long   addr[64];
    unsigned long   attr[16];
} spmp_cfg_t;


typedef enum {
    SPMP_RANGE_OFF      = (0 << 3),
    SPMP_RANGE_TOR      = (1 << 3),
    SPMP_RANGE_NA4      = (2 << 3),
    SPMP_RANGE_NAPOT    = (3 << 3),
} spmp_range_t;


typedef enum {                      
    SPMP_ATTR_OFF       = 0x00,        // ------ 
    SPMP_ATTR_RO        = 0x01,        // ---r--
    SPMP_ATTR_WO        = 0x02,        // rw-r--
    SPMP_ATTR_RW        = 0x03,        // ---rw-
    SPMP_ATTR_XO        = 0x04,        // -----x 
    SPMP_ATTR_RX        = 0x05,        // ---r-x
    SPMP_ATTR_WX        = 0x06,        // rw-rw-
    SPMP_ATTR_RWX       = 0x07,        // ---rwx
    SPMP_ATTR_RSVD      = 0x80,        // reserved 
    SPMP_ATTR_SRO       = 0x81,        // r-----
    SPMP_ATTR_SWO       = 0x82,        // --x--x
    SPMP_ATTR_SRW       = 0x83,        // rw----
    SPMP_ATTR_SXO       = 0x84,        // --x---
    SPMP_ATTR_SRX       = 0x85,        // r-x---
    SPMP_ATTR_SWX       = 0x86,        // r-x--x
    SPMP_ATTR_SRWX      = 0x87,        // r--r--
} spmp_attr_t;


// NAPOT region size = 2^__power__
#define NAPOT(__base__, __power__)      ((__base__ & (0xFFFFFFFE << (__power__ - 3))) | (0xFFFFFFFF >> (35 - __power__)))
#define CAST2TOR(__addr__)             ((unsigned long)(&__addr__))


/* RISC-V S-mode PMP API */
extern ret_t spmp_set_entry    (spmp_cfg_t *config, unsigned long index, unsigned long addr, unsigned long attr);
extern ret_t spmp_set_switch   (spmp_cfg_t *config, unsigned long switch0, unsigned long switch1 );
extern ret_t spmp_config_dump  (spmp_cfg_t *config);
extern ret_t spmp_config_show  (spmp_cfg_t *config);
extern ret_t spmp_config_apply (spmp_cfg_t *config);
extern ret_t spmp_switch       (v2u32_t mask);
