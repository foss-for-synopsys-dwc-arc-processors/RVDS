/// @file   main.c
/// @brief  RISC-V Test Monitor - MPU L1/L2 (VS/HS-mode) demo application #4
///         MPU L1 (VS-mode), static memory map, physical address space
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

/* VS/HS-mode map - static non-translated regions */

//          L2      L1
// .code    r-x     r-x--x
// .rdata   r--     r--r--
// .data    rw-     rw-rw-
// .mmio    rw-     rw----
// .htif    rw-     rw-rw-

/* Static physical (protected) and virtual (translated) memory map */

static const unsigned long PTE[][2] = {

        /* L2 (HS-mode) configuration */
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

        /* L1 (VS/VU-mode) configuration */
        { (long)&__text_base   + 0, (long)&__text_size   + SMPU_ATTR_SXR },  // [5]
        { (long)&__rodata_base + 0, (long)&__rodata_size + SMPU_ATTR_SRO },  // [6]
        { (long)&__data_base   + 0, (long)&__data_size   + SMPU_ATTR_SRW },  // [7]
        { (long)&__mmio_base   + 0, (long)&__mmio_size   + SMPU_ATTR_RW  },  // [8]
        { (long)&__htif_base   + 0, (long)&__htif_size   + SMPU_ATTR_SRW },  // [9]
        //         |             |           |                |
        //         |             |           |                +-- region permissions
        //         |             |           +-- protected region size 
        //         |             +-- protected region marker (if !=0 then translated)
        //         +-- protected region base address

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

        /* Display memory map exported from linker script */
        memory_map();

        tmon_call(TMON_FID_PRIV, S_MODE);      // to S

        /* Configure amd enable L2 (HS-mode MPU) */

        hmpu_group_config( 0, 5, PTE );
        hmpu_group_enable( 1, 0x0000001F );    // enable shared S/U-mode protected regions

        hmpu_config_show(-1);

        __csrs(CSR_HSTATUS, 1 << 12);

    /* Switch to VS-mode, check access to shared memory */
    CASE(1);

        tmon_call(TMON_FID_PRIV, VS_MODE);      // to VS

        smpu_disable();

        smpu_group_config( 0, 5, &PTE[5] );
        smpu_group_enable( 1, 0x0000001F );    // enable shared S/U-mode protected regions

        smpu_config_show(-1);

    /* Check privileges */
    CASE(2);

        tmon_call(TMON_FID_PRIV, VU_MODE);      // to VU
        tmon_call(TMON_FID_PRIV, VS_MODE);      // to VS
        // tmon_call(TMON_FID_PRIV, U_MODE);       // to U, must configure L1 (S-mode) first
        tmon_call(TMON_FID_PRIV, S_MODE);       // to S


        tmon_call(TMON_FID_PRIV, M_MODE);       // to M

        exit(0);
}
