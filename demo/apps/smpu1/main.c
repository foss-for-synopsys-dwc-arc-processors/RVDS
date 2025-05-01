/// @file   main.c
/// @brief  RISC-V Test Monitor - S-mode MPU test application #1
///         MPU L1 (S-mode), static virtual memory map 

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
        { (long)&__text_base   + 0, (long)&__text_size   + SMPU_ATTR_SXR },  // [0]
        { (long)&__rodata_base + 0, (long)&__rodata_size + SMPU_ATTR_SRO },  // [1]
        { (long)&__data_base   + 0, (long)&__data_size   + SMPU_ATTR_RWX },  // [2]
        { (long)&__mmio_base   + 0, (long)&__mmio_size   + SMPU_ATTR_SRW },  // [3]
        { (long)&__htif_base   + 0, (long)&__htif_size   + SMPU_ATTR_SRW },  // [4]
        //         |             |           |                |
        //         |             |           |                +-- region permissions
        //         |             |           +-- protected region size 
        //         |             +-- protected region marker (if !=0 then translated)
        //         +-- protected region base address

        /* Shared S/U-mode translated regions */
        { 0x00080000           +12, (long)&__stack_base - 0x2000 + SMPU_ATTR_SXR },  // [5]
        { 0x000A0000           +12, (long)&__stack_base - 0x4000 + SMPU_ATTR_SRO },  // [6]
        { 0x000C0000           +12, (long)&__stack_base - 0x6000 + SMPU_ATTR_SRW },  // [7]
        //  |                   |                       |          |
        //  |                   |                       |          +-- permissions
        //  |                   |                       +-- physical address (page number)
        //  |                   +== translation page size
        //  +-- virtual address (page number)
        //  

        /* Test crossing regions */      
        { 0x000C0000          +12, (long)&__stack_base - 0x8000 + SMPU_ATTR_SRW },  // [8] overlaps translated region [7] in VA
        { 0x000C0000          +0,  0x1000 - 32                  + SMPU_ATTR_SRW },  // [9] overlaps translated region [7] in VA
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

        /* Make sure SMPU is disabled and it's safe to pass control to S-mode (RWX by default) */

        smpu_disable();                         

        /* Test needs S-mode privileges */

        tmon_call(TMON_FID_PRIV, 1);      // to S

        /* Display memory map exported from linker script */
        memory_map();

        /* Configure amd enable SMPU protected regions */

        smpu_group_config( 0, 10, PTE );
        smpu_group_enable(1, 0x0000001F);    // enable shared S/U-mode protected regions
        smpu_config_show(-1);

        /* Declare and initialize test variables */

        volatile long* vp0 = (long*)(0x000a0000);         // r-- r--
        volatile long* vp1 = (long*)(0x000c0000);         // rw- rw-
        fp_t           vf0 =  (fp_t)(0x00080000);         // r-x r--
        volatile long  d0;
      
        for (int i=0; i<32; i++) {
                // copy test function to physical memory location
                ((unsigned long *)((unsigned long)&__stack_base - 0x2000))[i] = ((unsigned long*)(func))[i];
        }

        /* Enable virtual address space */

        smpu_group_enable(1, 0x000000E0);
        smpu_config_show(-1);
        
        /* Run few tests */

        /* CASE: Check read across translated regions */
        CASE(1);        

        d0 = *vp0;
        d0 = *vp1;

        // tmon_call(TMON_FID_EXPECT, 13);         // set expectation on trap #13 (data store fault)
        d0 = *(long*)vf0;
        tmon_call(TMON_FID_VERIFY, 13);          // check expectations
        

        /* CASE: Check write across translated regions */
        CASE(2);

        tmon_call(TMON_FID_EXPECT, 15);         // set expectation on trap #15 (data store fault)
        *vp0 = d0;
        tmon_call(TMON_FID_VERIFY, 15);         // check expectations

        *vp1 = d0;

        tmon_call(TMON_FID_EXPECT, 15);         // set expectation on trap #15 (data store fault)
        *(long*)(vf0) = d0;
        tmon_call(TMON_FID_VERIFY, 15);         


        /* CASE: Check execute across translated regions */
        CASE(3);        // try execute

        tmon_call(TMON_FID_EXPECT, 12);         // set expectation on trap #12 (fetch)
        ((fp_t)(vp0))();
        tmon_call(TMON_FID_VERIFY, 12);         


        tmon_call(TMON_FID_EXPECT, 12);         // set expectation on trap #12 (fetch)
        ((fp_t)(vp1))();
        tmon_call(TMON_FID_VERIFY, 12);         


        vf0();

        /* CASE: Check region crossing (translated-x-translated) */
        CASE(4);

        smpu_group_enable(1, 0x00000100);       // enable crossing region
        smpu_config_show(-1);

        tmon_call(TMON_FID_EXPECT, 14);
        *vp1 = d0;
        tmon_call(TMON_FID_VERIFY, 14);         

        smpu_group_enable(0, 0x00000100);       // disable crossing region
        

        /* CASE: Check region crossing (translated-x-protected) */
        CASE(5);

        smpu_group_enable(1, 0x00000200);       // enable crossing region
        smpu_config_show(-1);

        tmon_call(TMON_FID_EXPECT, 14);
        *vp1 = d0;
        tmon_call(TMON_FID_VERIFY, 14);         

        smpu_group_enable(0, 0x00000200);       // disable crossing region


        /* CASE: Check enabled region reconfiguration */
        CASE(6);

        smpu_group_enable(1, 0x00000300);       // enable test regions

        tmon_call(TMON_FID_EXPECT, 2);          // expect illegal instruction trap
        __icsrw( ICSR_SMPU_BASE + (9 << 1) + 0, 0x0 );  // try write to smpuaddr9
        tmon_call(TMON_FID_VERIFY, 2);

        tmon_call(TMON_FID_EXPECT, 2);          // expect illegal instruction trap
        __icsrw( ICSR_SMPU_BASE + (9 << 1) + 1, 0x0 );  // try write to smpuconf9
        tmon_call(TMON_FID_VERIFY, 2);

        /* Finish and exit */
        
        tmon_call(TMON_FID_PRIV, 3);            // to M

        exit(0);
}
