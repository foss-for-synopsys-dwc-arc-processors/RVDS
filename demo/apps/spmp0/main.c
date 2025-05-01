/// @file   main.c
/// @brief  RISC-V Virtual Platform - S-mode PMP test/demo application, static memory map 

#include "tmon.h"
#include "spmp.h"

// Exports from linker script
#if 0
extern volatile long __htif_base[];
extern volatile long __mmio_base[];
extern long __stack_top;
extern long __rodata_top;
extern long __text_top;
extern long __sram_base;
#else
extern volatile long __htif_base[], __htif_size;
extern volatile long __mmio_base[], __mmio_size;
extern long __data_base, __data_size;
extern long __rodata_base, __rodata_size;
extern long __text_base, __text_size;
extern long __sram_base, __sram_size;
extern long __heap_base, __heap_size;
extern long __stack_base, __stack_size;
#endif

// Shadow SPMP configuration 
static spmp_cfg_t spmp_cfg;


// test call type
typedef void (*fp_t)(void);


int main(void)
{
        ret_t ret;

        printf("%s: S-mode PMP test application, static memory map\n", __func__);

        printf(" htif       : 0x%lx\n", (long)(&__htif_base)   + (long)(&__htif_size)   );
        printf(" mmio       : 0x%lx\n", (long)(&__mmio_base)   + (long)(&__mmio_size)   );

        printf(" data/stack : 0x%lx\n", (long)(&__stack_base)  + (long)(&__stack_size)  );
        printf(" ro data    : 0x%lx\n", (long)(&__rodata_base) + (long)(&__rodata_size) );
        printf(" text       : 0x%lx\n", (long)(&__text_base)   + (long)(&__text_size)   );

        printf(" SRAM start : 0x%lx\n", (long)(&__sram_base) );

        /* System memory map */

        //             start:end                size
        // -----------------|------------------|----
        // SRAM: 0x0000_0000:0x000F_FFFF        1M 
        // MMIO: 0x0200_0000:0x0200_FFFF        64K
        // HTIF: 0x0201_0000:0x0201_0FFF        4K 

        /* M/S/U shared memory map - test application shared regions */

        // .text                        r-x--x  __text_top        TOR  
        // .rdata .rodata .srodata      r--r--  __rodata_top      TOR
        // .bss .sbss .data .stack      rw-rw-  __stack_top       TOR
        // .mmio                        rw-rw-  02000000:0200FFFF NAPOT        
        // .htif                        rw-rw-  02010000:02010FFF NAPOT

        // MMIO         NAPOT  64K  rw-rw-
        spmp_set_entry(&spmp_cfg, 0, NAPOT(0x02000000, 15),  SPMP_RANGE_NAPOT | SPMP_ATTR_WX);

        // HTIF         NAPOT   4K  rw-rw-
        spmp_set_entry(&spmp_cfg, 1, NAPOT(0x02010000, 12),  SPMP_RANGE_NAPOT | SPMP_ATTR_WX);

        // text         TOR     ??  r-x--x        
        spmp_set_entry(&spmp_cfg, 4, (long)(&__text_base) + (long)(&__text_size),   SPMP_RANGE_TOR   | SPMP_ATTR_SWX);

        // rdata        TOR     ??  r--r--
        spmp_set_entry(&spmp_cfg, 5, (long)(&__rodata_base) + (long)(&__rodata_size), SPMP_RANGE_TOR   | SPMP_ATTR_SRWX);

        // data         TOR     ??  rw-rw-
        spmp_set_entry(&spmp_cfg, 6, (long)(&__stack_base) + (long)(&__stack_size),  SPMP_RANGE_TOR   | SPMP_ATTR_WX);

        // enable configured SPMP entries
        spmp_set_switch(&spmp_cfg, 0x00000073, 0x00000000);
        
        /* Enable SPMP */

        ret = spmp_config_apply(&spmp_cfg);     // apply shadow copy to CSRs             

        ret = spmp_config_dump(&spmp_cfg);      // dump from CSRs
        ret = spmp_config_show(&spmp_cfg);      // display actual SPMP configuration

        /* Declare "bad" pointers */

        volatile long *p0 = (long*)((long)(&__text_base) + (long)(&__text_size) - 32);  // S-mode read-execute region; U-mode execute only)
        volatile long *p1 = (long*)((long)(&__text_base) + (long)(&__text_size) + 32);  // access denied (free protected space)
        fp_t f0  = (fp_t)((long)(&__stack_base) + (long)(&__stack_size) - 32);   	// noe) (& execute) (se) (&tack))

        /* Continue in U-mode */

        tmon_call(TMON_FID_PRIV, U_MODE);       // to U        

    /* CASE: violate U-mode write permission */
    CASE(1);
        tmon_call(TMON_FID_EXPECT, 15);         // set expectation on trap #15 (data store fault)
        *p0 = 0;                                // read-only region write attempt 
        tmon_call(TMON_FID_VERIFY, 15);         // check expectation occured, error if not

    /* CASE: violate U-mode write permission */
    CASE(2);
        tmon_call(TMON_FID_EXPECT, 15);         // set expectation on trap #15 (data store fault)
        *p1 = 0;                                // access denied region write attempt 
        tmon_call(TMON_FID_VERIFY, 15);         // check expectation occured, error if not

    /* CASE: violate U-mode fetch permission (read-wrire region) */
    CASE(3);
        tmon_call(TMON_FID_EXPECT, 12);
        f0();
        tmon_call(TMON_FID_VERIFY, 12);

    /* CASE: violate U-mode read permission (execute-only region) */
    CASE(4);
        tmon_call(TMON_FID_EXPECT, 13);
        long read = *p0;
        tmon_call(TMON_FID_VERIFY, 13);


        tmon_call(TMON_FID_PRIV, S_MODE);       // to S

    /* CASE: violate S-mode write permission */
    CASE(5);
        tmon_call(TMON_FID_EXPECT, 15);         // set expectation on trap #15 (data store fault)
        *p0 = 0;                                // read-only region write attempt 
        tmon_call(TMON_FID_VERIFY, 15);         // check expectation occured, error if not

    /* CASE: violate U-mode write permission */
    CASE(6);
        tmon_call(TMON_FID_EXPECT, 15);         // set expectation on trap #15 (data store fault)
        *p1 = 0;                                // access denoied region write attempt 
        tmon_call(TMON_FID_VERIFY, 15);         // check expectation occured, error if not

    /* CASE: violate S-mode fetch permission */
    CASE(7);
        tmon_call(TMON_FID_EXPECT, 12);
        f0();
        tmon_call(TMON_FID_VERIFY, 12);


        tmon_call(TMON_FID_PRIV, M_MODE);       // to M
        
        exit(0);
}
