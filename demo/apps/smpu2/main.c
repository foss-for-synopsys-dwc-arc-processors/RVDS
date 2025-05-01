/// @file   main.c
/// @brief  RISC-V Test Monitor - MPU L2 (HS-mode) demo application #2
///         MPU L2 (HS-mode), static memory map, physical address space

#include "arch.h"
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


// test function type
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

/* S-mode virtual memory map - static translated regions */

// .va.code  0x80000            r-x--x
//           0x90000            ------
// .va.rdata 0xA0000            r--r--
//           0xB0000            ------
// .va.data  0xC0000            rw-rw-


/* Static physical (protected) and virtual (translated) memory map */

static const unsigned long PTE[][2] = {

        /* Shared S/U-mode protected regions */
        { (long)&__text_base   + 0, (long)&__text_size   + HMPU_ATTR_SRX },  // [0]
        { (long)&__rodata_base + 0, (long)&__rodata_size + HMPU_ATTR_SRO },  // [1]
        { (long)&__data_base   + 0, (long)&__data_size   + HMPU_ATTR_SRW },  // [2]
        { (long)&__mmio_base   + 0, (long)&__mmio_size   + HMPU_ATTR_SRW },  // [3]
        { (long)&__htif_base   + 0, (long)&__htif_size   + HMPU_ATTR_SRW },  // [4]
        //         |             |           |                |
        //         |             |           |                +-- region permissions
        //         |             |           +-- protected region size 
        //         |             +-- protected region marker (if !=0 then translated)
        //         +-- protected region base address

        /* Shared S/U-mode translated regions */
        { 0, 0 },  // [5]
        { 0, 0 },  // [6]
        { 0, 0 },  // [7]

};


/// @name  func()
/// @brief "relocatable" test function
void __attribute__((noinline, optimize("O0"))) func(void) {
        return;
}


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

        printf("%s: S-mode MPU test application, static translated memory map\n", __func__);

        /* disable SMPU L1/L2, so it's safe to pass control to S-mode (RWX by default) */

        smpu_disable();                         
        hmpu_disable();  

        tmon_call(TMON_FID_PRIV, S_MODE);      // to S

        /* Display memory map exported from linker script */
        memory_map();

        /* Configure amd enable SMPU protected regions */

        hmpu_disable();                         
        hmpu_group_config( 0, 6, PTE );
        hmpu_group_enable(1, 0x0000001F);    // enable shared S/U-mode protected regions

        hmpu_config_show(-1);

        /* Declare and initialize test variables */

        volatile long *p0 = (long*)(&__text_base + 32);   // r-x r-x
        volatile long *p1 = (long*)(&__sram_size + 32);   // --- --- 
        fp_t f0  =  (fp_t)(&__stack_base + 32);           // rw- rw-


    /* Switch to VS-mode, check access to shared memory */
    CASE(1);

        tmon_call(TMON_FID_PRIV, VS_MODE);      // to VS

    /* CASE: violate S-mode permissions */
    CASE(2);        

        tmon_call(TMON_FID_EXPECT, 15);         // expect trap #15 (data store fault)
        tmon_call(TMON_FID_EXPECT, 12);         // expect trap #12 (instr fetch fault)
        tmon_call(TMON_FID_EXPECT, 15);         // expect trap #15 (data store fault)

        *p0 = 0;                                // write to r-x
        f0();                                   // execute from rw-
        *p1 = 0;                                // write to ---

        tmon_call(TMON_FID_VERIFY, 0);          // check expectation occurred, error if not

        /* U-mode protection */

    /* Switch to VU-mode, check access to shared memory */
    CASE(2);

        tmon_call(TMON_FID_PRIV, VU_MODE);            // to VU        

    /* CASE: violate VU-mode write permission */
    CASE(3);        

        tmon_call(TMON_FID_EXPECT, 15);         // expect trap #15 (data store fault)
        tmon_call(TMON_FID_EXPECT, 12);         // expect trap #12 (instr fetch fault)
        tmon_call(TMON_FID_EXPECT, 15);         // expect trap #15 (data store fault)

        *p0 = 0;                                // write to r-x
        f0();                                   // execute from rw-
        *p1 = 0;                                // write to ---

        tmon_call(TMON_FID_VERIFY, 0);          // check expectation occurred, error if not

    /* Finalize */
        tmon_call(TMON_FID_PRIV, 3);            // to M

        exit(0);
}
