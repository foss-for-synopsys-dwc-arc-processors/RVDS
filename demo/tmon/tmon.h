/// @file   tmon.h
/// @brief  RISC-V Virtuial Platform - test monitor implementation

#pragma once

#include <stdint.h>

#include "semihost.h"

/* Linker script exports */

extern volatile long __mmio_base[];
extern volatile long __mmsi_base[];
extern volatile long __smsi_base[];

/* Test Monitor Event Queue */

typedef struct tmon_queue_s {
    unsigned long *head;
    unsigned long *tail;
    unsigned long *buff;
    unsigned long  size;
} tmon_queue_t;

/* Test Monitor data types */

typedef signed long long    s64_t;
typedef signed long         s32_t;
typedef signed short        s16_t;
typedef signed char         s8_t;

typedef unsigned long long  u64_t;
typedef unsigned long       u32_t;
typedef unsigned short      u16_t;
typedef unsigned char       u8_t;

typedef union v64_u {
    u8_t        u8[8];
    u16_t       u16[4];
    u32_t       u32[2];
    u64_t       u64;
} v2u32_t, v4u16_t, v8u8_t, *p2u32_t, *p4u16_t, *p8u8_t;

typedef union v32_u {
    u8_t        u8[4];
    u16_t       u16[2];
    u32_t       u32;
} v2u16_t, v4u8_t, *p2u16_t, *p4u8_t;

/* Trap Defines */

enum trap_mode_e {
    TRAP_MODE_DIRECT        = 0,
    TRAP_MODE_VECTORED      = 1,
    TRAP_MODE_NESTED        = 3,
};


enum trap_eid_e {
    TRAP_EID_ILLEGAL_INST  = 2,
    TRAP_EID_UCALL         = 8,
    TRAP_EID_SCALL         = 9,
    TRAP_EID_VCALL         = 10,
    TRAP_EID_MCALL         = 11,
    TRAP_EID_FETCH_FAULT   = 12,
    TRAP_EID_LOAD_FAULT    = 13,
    TRAP_EID_CROSS_FAULT   = 14,
    TRAP_EID_STORE_FAULT   = 15,
};

enum trap_iid_e {
    TRAP_IID_SSWI          = 1,          // 33 maj1  S-mode software interrupt
    TRAP_IID_VSWI          = 2,          // 34 maj2  VS-mode software interrupt, always delegated
    TRAP_IID_MSWI          = 3,          // 35 maj3  M-mode software interrupt

    TRAP_IID_STMR          = 5,          // 37 maj5  S-mode timer interrupt
    TRAP_IID_VTMR          = 6,          // 38 maj6  VS-mode timer interrupt, always delegated
    TRAP_IID_MTMR          = 7,          // 39 maj7  M-mode time interrupt

    TRAP_IID_SEXT          = 9,          // 41 maj9  S-mode external interrupt
    TRAP_IID_VEXT          = 10,         // 42 maj10 VS-mode external interrupt, always delegated
    TRAP_IID_MEXT          = 11,         // 43 maj11 M-mode external interrupt 

    TRAP_IID_GEXT          = 12,         // 44 maj12 Guest external interrupt
    TRAP_IID_COV           = 13,         // 45 maj13 Counter overflow interrupt
};


/* Test Monitor Privilege Modes */

extern const char *priv_s[8];

#define  U_MODE      ((void*)(0))
#define  S_MODE      ((void*)(1))
#define  M_MODE      ((void*)(3))
#define VU_MODE      ((void*)(4))
#define VS_MODE      ((void*)(5))
#define VM_MODE      ((void*)(7))

/* Test monitor environment function calls  */

typedef enum fid_e {
    TMON_FID_FAIL   = 0,
    TMON_FID_EXIT   = 1,
    TMON_FID_CSRR   = 2,
    TMON_FID_CSRW   = 3,
    TMON_FID_CSRS   = 4,
    TMON_FID_CSRC   = 5,
    TMON_FID_ICSRR  = 6,
    TMON_FID_ICSRW  = 7,
    TMON_FID_ICSRS  = 8,
    TMON_FID_ICSRC  = 9,
    TMON_FID_PRIV   = 10,   
    TMON_FID_EXPECT = 11,       // add trap to the expected events list 
    TMON_FID_VERIFY = 12,       // check expectations
    TMON_FID_MSWI   = 13,       // assert/de-assert M-mode software interrupt
    TMON_FID_SSWI   = 14,       // assert/de-assert S-mode software interrupt  
    TMON_FID_MMSI   = 15,       // send M-mode MSI (external) interrupt
    TMON_FID_SMSI   = 16,       // send S-mode MSI (external) interrupt
    TMON_FID_CB     = 17,       // link user callback to the specified trap handler
} fid_t;


/* Test monitor exception/interrupt vectors */

extern void* m_trap_vector[];
extern void* s_trap_vector[];

/* User trap-call-back vectors */

extern void* m_trap_callback[];
extern void* s_trap_callback[];

/*
    Vector Table:

    index   mode=0      mode=1              mode=3
    -----------------------------------------------------------------
    0       exc0        exc0                exc0
    1       exc1        exc1                exc1
    ..      
    31      exc31       exc31               exc31
    32      maj0        exc   -> vect0      exc   -> vect0
    33      maj1        maj1  -> vect1      ext1  -> vect1
    ..
    63      maj31       maj31 -> vect31     ext31 -> vect31
    64      ext0        ext0                ext32 -> vect32 (chained)
    65      ext1        ext1                ext33 -> vect33 (chained)
    ..
    95      ext31       ext31               ext63 -> vect63 (chained)
    -----------------------------------------------------------------
exc traps   32          32                  32
maj irqs    31          31                  -
ext irqs    32          32                  63
    {x}tvec &trap_wrap  &IVT[32]            &IVT[32]
*/


typedef struct {
    unsigned long   a0;
    unsigned long   a1;
} ret_t;


/* printf.c */
extern int32_t  printf(const char* fmt, ...);
extern int32_t  puts(const char* str);

/* tmon.c */
extern void queue_append(unsigned long e);
extern unsigned long queue_pop(void);
extern unsigned long queue_is_empty(void );



/* tmon.c */
// extern ret_t _tmon_call(unsigned long fid, void *args);
// #define tmon_call       _tmon_call   

#define tmon_call(__fid__, __args__)  {                                 \
    register unsigned long a0 asm ("a0") = (unsigned long)(__fid__);    \
    register unsigned long a1 asm ("a1") = (unsigned long)(__args__);   \
    asm volatile ( "ecall \n" : "+r"(a0), "+r"(a1) : : "memory" );      \
}


/* helper macros */

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define VA_ARGS(...)            , ##__VA_ARGS__

#define CASE(__idx__)           printf("%s:%d:%s() " ANSI_COLOR_GREEN "CASE #%d" ANSI_COLOR_RESET "\n", __FILE__, __LINE__, __func__, __idx__)
#define TRACE(__fmt__, ...)     printf("%s:%d:%s() " __fmt__, __FILE__, __LINE__, __func__ VA_ARGS(__VA_ARGS__))
#define ERROR(__fmt__, ...)     printf("%s:%d:%s():" ANSI_COLOR_RED "error" ANSI_COLOR_RESET ": " __fmt__, __FILE__, __LINE__, __func__ VA_ARGS(__VA_ARGS__))
#define WARNING(__fmt__, ...)   printf("%s:%d:%s():" ANSI_COLOR_MAGENTA "warning" ANSI_COLOR_RESET ": " __fmt__, __FILE__, __LINE__, __func__ VA_ARGS(__VA_ARGS__))

