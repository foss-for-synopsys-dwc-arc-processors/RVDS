/// @file   main.c
/// @brief  RISC-V Virtual Platform - S-mode MPU test application #1, static protected memory map 

#include "tmon.h"
#include "smpu.h"

// Exports from linker script
extern volatile long __htif_base[], __htif_size;
extern volatile long __mmio_base[], __mmio_size;
extern long __data_base, __data_size;
extern long __rodata_base, __rodata_size;
extern long __text_base, __text_size;
extern long __sram_base, __sram_size;
extern long __heap_base, __heap_size;
extern long __stack_base, __stack_size;


// test call type
typedef void (*fp_t)(void);


/* System memory map */

//             start:end                size
// -----------------|------------------|----
// SRAM: 0x0000_0000:0x000F_FFFF        1M 
// MMIO: 0x0200_0000:0x0200_FFFF        64K
// HTIF: 0x0201_0000:0x0201_0FFF        4K 

/* M/S/U shared memory map - test application shared regions */

//  sections                 permissions
// ------------------------|-------------
// .text                        r-x--x            
// .rdata .rodata .srodata      r--r--
// .data                        rw-rw-
//   .bss .sbss .data           
//   .heap                               
//   .stack                              
// .mmio                        rw-rw-          
// .htif                        rw-rw-  

static const unsigned long PTE[][2] = {

        /* Shared S/U-mode protected regions */
        { (long)&__text_base    + 0, (long)&__text_size   + SMPU_ATTR_SXR  },  // [0]
        { (long)&__rodata_base  + 0, (long)&__rodata_size + SMPU_ATTR_SRO  },  // [1]
        { (long)&__data_base    + 0, (long)&__data_size   + SMPU_ATTR_SRW  },  // [2]
        { (long)&__mmio_base    + 0, (long)&__mmio_size   + SMPU_ATTR_SRW  },  // [3]
        { (long)&__htif_base    + 0, (long)&__htif_size   + SMPU_ATTR_SRW  },  // [4]
        //         |              |           |             |
        //         |              |           |             +-- region permissions
        //         |              |           +-- protected region size 
        //         |              +-- protected region marker (if !=0 then translated)
        //         +-- protected region base address
        { 0, 0 },                                                              // [5]
        { 0, 0 },                                                              // [6]
        { 0, 0 },                                                              // [7]

};


/// @name  memory_map()
/// @brief display memory map as exported from linker script
static void memory_map(void) {

        printf(" REGION     BASE - TOP : SIZE\n");

        printf(" htif       0x%lx - 0x%lx : 0x%lx\n", (long)(&__htif_base),   (long)(&__htif_base)   + (long)(&__htif_size)   + 32, (long)(&__htif_size)   + 32);
        printf(" mmio       0x%lx - 0x%lx : 0x%lx\n", (long)(&__mmio_base),   (long)(&__mmio_base)   + (long)(&__mmio_size)   + 32, (long)(&__mmio_size)   + 32);


        printf(" stack      0x%lx - 0x%lx : 0x%lx\n", (long)(&__stack_base),  (long)(&__stack_base)  + (long)(&__stack_size)  + 32, (long)(&__stack_size)  + 32);
        printf(" heap       0x%lx - 0x%lx : 0x%lx\n", (long)(&__heap_base),   (long)(&__heap_base)   + (long)(&__heap_size)   + 32, (long)(&__heap_size)   + 32);

        printf(" data       0x%lx - 0x%lx : 0x%lx\n", (long)(&__data_base),   (long)(&__data_base)   + (long)(&__data_size)   + 32, (long)(&__data_size)   + 32);
        printf(" r/o data   0x%lx - 0x%lx : 0x%lx\n", (long)(&__rodata_base), (long)(&__rodata_base) + (long)(&__rodata_size) + 32, (long)(&__rodata_size) + 32);
        printf(" text       0x%lx - 0x%lx : 0x%lx\n", (long)(&__text_base),   (long)(&__text_base)   + (long)(&__text_size)   + 32, (long)(&__text_size)   + 32);

        printf(" SRAM       0x%lx::0x%lx\n", (long)(&__sram_base),   (long)(&__sram_size)  );
}


int main(void)
{

        printf("%s: S-mode MPU test application, static protected memory map\n", __func__);


        /* Make sure SMPU is disabled and it's safe to pass control to S-mode (RWX by default) */

        smpu_disable();                         

        /* Test needs S-mode privileges */

        tmon_call(TMON_FID_PRIV, S_MODE);

        /* Display memory map exported from linker script */
        memory_map();

        /* Configure amd enable SMPU protected regions */

        smpu_group_config( 0, 6, PTE );
        smpu_group_enable(1, 0x0000001F);       // enable S/U-mode protection
        smpu_config_show(-1);

        /* Declare and initialize "bad" pointers to test */

        volatile long *p0 = (long*)(&__text_base + 32);   // S-mode read-execute region; U-mode execute only)
        volatile long *p1 = (long*)(&__sram_size + 32);   // access denied (free protected space)
        fp_t f0  =  (fp_t)(&__stack_base + 32);           // no execute (stack)


        tmon_call(TMON_FID_PRIV, S_MODE);

        /* CASE: violate S-mode write permission */
        CASE(1);        

        tmon_call(TMON_FID_EXPECT, 15);         // set expectation on trap #15 (data store fault)
        *p0 = 0;                                // read-only region write attempt 
        tmon_call(TMON_FID_VERIFY, 15);         // check expectation occurred, error if not

        /* CASE: violate U-mode write permission */
        CASE(2);        

        tmon_call(TMON_FID_EXPECT, 15);         // set expectation on trap #15 (data store fault)
        *p1 = 0;                                // access denied region write attempt 
        tmon_call(TMON_FID_VERIFY, 15);         // check expectation occurred, error if not

        /* CASE: violate S-mode fetch permission */
        CASE(3);        

        tmon_call(TMON_FID_EXPECT, 12);
        f0();
        tmon_call(TMON_FID_VERIFY, 12);

        /* U-mode protection */

        tmon_call(TMON_FID_PRIV, 0);            // to U        

        /* CASE: violate U-mode write permission */
        CASE(4);        

        tmon_call(TMON_FID_EXPECT, 15);         // set expectation on trap #15 (data store fault)
        *p0 = 0;                                // read-only region write attempt 
        tmon_call(TMON_FID_VERIFY, 15);         // check expectation occurred, error if not

        /* CASE: violate U-mode write permission */
        CASE(5);        

        tmon_call(TMON_FID_EXPECT, 15);         // set expectation on trap #15 (data store fault)
        *p1 = 0;                                // access denied region write attempt 
        tmon_call(TMON_FID_VERIFY, 15);         // check expectation occurred, error if not

        /* CASE: violate U-mode fetch permission (read-write region) */
        CASE(6);        

        tmon_call(TMON_FID_EXPECT, 12);
        f0();
        tmon_call(TMON_FID_VERIFY, 12);

        /* CASE: violate U-mode read/write permission (execute-only region) */
        CASE(8);        

        tmon_call(TMON_FID_EXPECT, 13);
        tmon_call(TMON_FID_EXPECT, 15);
        long read = *p0;
        *p0 = read;
        tmon_call(TMON_FID_VERIFY, 13);
        tmon_call(TMON_FID_VERIFY, 15);


        tmon_call(TMON_FID_PRIV, 3);            // to M

        exit(0);
}
