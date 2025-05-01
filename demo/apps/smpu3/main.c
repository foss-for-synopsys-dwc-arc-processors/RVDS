/// @file   main.c
/// @brief  RISC-V Test Monitor - S-mode MPU demo application #3
///         MPU L2 (HS-mode), static memory map, physical and virtual address space

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

/* HS/VS/VU shared memory map - test application shared regions */

//  sections                 permissions
// ------------------------|-------------
// .text                        r-x            
// .rdata .rodata .srodata      r--
// .data                        rw-
//   .bss .sbss .data           
//   .heap                               
//   .stack       
// .test                        var
//                       
// .mmio                        rw-          
// .htif                        rw-  

/* VS/VU-mode virtual memory map - static translated regions */

//  name      start : end      VS/VU
// .va.code  0x80000:0x80FFF    r-x     // test r-x v-page
//           0x81000:0x9FFFF    ---     // not allocated
// .va.rdata 0xA0000:0xA0FFF    r--     // test r-- v-page
//           0xA1000:0xBFFFF    ---     // not allocated
// .va.data  0xC0000:0xC0FFF    rw-     // test r-x v-page
//           0xC1000:0xCFFFF    ---     // not allocated
// .va.exec  0xD0000:0xD0FFF    rw-     // test --x v-page


/* Static physical (protected) and virtual (translated) memory map */

static const unsigned long PTE[][2] = {

        /* Shared VS/VU-mode protected (PA) regions */

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

        /* Shared VS/VU-mode translated (VA) regions */
        { 0x00080000           +12, (long)&__stack_base - 0x2000 + HMPU_ATTR_SRX },  // [5]
        { 0x000A0000           +12, (long)&__stack_base - 0x4000 + HMPU_ATTR_SRO },  // [6]
        { 0x000C0000           +12, (long)&__stack_base - 0x6000 + HMPU_ATTR_SRW },  // [7]
        { 0x000D0000           +12, (long)&__stack_base - 0x8000 + HMPU_ATTR_SXO },  // [8]
        //  |                   |                       |          |
        //  |                   |                       |          +-- permissions
        //  |                   |                       +-- physical address (page number)
        //  |                   +== translation page size
        //  +-- virtual address (page number)
        //  

};


/// @name  func()
/// @brief "position independent" test function
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

        printf("%s: SMPU L2 (HS-mode) test application, static physical/virtual address for VM\n", __func__);

        /* disable SMPU L1/L2, so it's safe to pass control to S-mode (RWX by default) */

        smpu_disable();                         
        hmpu_disable();                         

        tmon_call(TMON_FID_PRIV, S_MODE);      // to S

        /* Display memory map exported from linker script */
        memory_map();

        /* Copy test SMPU configuration to the iCSRs */

        hmpu_group_config( 0, 9, PTE );


        /* Declare and initialize test variables */

        volatile long* vp0 = (long*)(0x000a0000);         // r-- 
        volatile long* vp1 = (long*)(0x000c0000);         // rw- 
        fp_t           vf0 =  (fp_t)(0x00080000);         // r-x 
        fp_t           vf1 =  (fp_t)(0x000d0000);         // --x 
        volatile long  d0;                                // stack storage 
      
        for (int i=0; i<32; i++) {
                // copy test function to physical memory location
                ((unsigned long *)((unsigned long)&__stack_base - 0x2000))[i] = ((unsigned long*)(func))[i];
        }

        
        /* Run few tests */
    
    /* CASE: Switch to VS-mode, check accesses to shared regions */
    CASE(0);       
        
        // enable not-translated shared VS/VU-mode protected regions
        hmpu_group_enable(1, 0x0000001F);    
        hmpu_config_show(-1);

        tmon_call(TMON_FID_PRIV, VS_MODE);      // to VS
        tmon_call(TMON_FID_PRIV, VU_MODE);      // to VU

    /* CASE: Check read (load) across translated regions */
    CASE(1);       

        tmon_call(TMON_FID_PRIV, S_MODE);      // to S

        // Enable virtual space 
        hmpu_group_enable(1, 0x000001FF);
        hmpu_config_show(-1);

        tmon_call(TMON_FID_PRIV, VU_MODE);      // to VU
        tmon_call(TMON_FID_PRIV, VS_MODE);      // to VS

    /* CASE: Check read (load) across translated regions */
    CASE(2);       
    
        d0 = *vp0;
        d0 = *vp1;
        d0 = *(long*)vf0;
        
    /* CASE: Check write across translated regions */
    CASE(3);

        *vp1 = d0;

        tmon_call(TMON_FID_EXPECT, 15);         // expect trap #15 (data store fault)
        *vp0 = d0;

        tmon_call(TMON_FID_EXPECT, 13);         // expect trap #13 (data load fault)
        d0 = *(unsigned long*)vf1;

        tmon_call(TMON_FID_EXPECT, 15);         // expect trap #15 (data store fault)
        *(long*)(vf0) = d0;

        tmon_call(TMON_FID_VERIFY, 0);         


    /* CASE: Check execute across translated regions */
    CASE(4);        // try execute

        tmon_call(TMON_FID_EXPECT, 12);         // expect trap #12 (fetch)
        ((fp_t)(vp0))();
        tmon_call(TMON_FID_VERIFY, 12);         

        tmon_call(TMON_FID_EXPECT, 12);         // expect trap #12 (fetch)
        ((fp_t)(vp1))();
        tmon_call(TMON_FID_VERIFY, 12);         

        vf0();



        /* Finish and exit */        
        tmon_call(TMON_FID_PRIV, M_MODE);      // to M

        exit(0);
}
