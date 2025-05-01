### @file   crt0.S
### @brief  Low level startup code for RISC-V Virtuial Platform

.section    ".text"

.global     _start
.global     _exit

#
.extern     __bss_start
.extern     __bss_end
.extern     __stack_top

# 
.extern     _m_trap_wrapper
.extern     _s_trap_wrapper

# .extern     main


.equ        SYSCALL_ADDR, 0x02010000

_start:
# set up trap entry point
    la      t0, _m_trap_wrapper
    csrw    mtvec, t0

    la      t0, _s_trap_wrapper
    csrw    stvec, t0

# disable interrupts
    li      t0, 0xb
    csrrc   zero, mstatus, t0   # disable global interrupt for all modes
    li      t0, 0x888
    csrrc   zero, mie, t0       # disable machine level interrupt

# clear registers
    li      x1, 0
    li      x2, 0
    li      x3, 0
    li      x4, 0
    li      x5, 0
    li      x6, 0
    li      x7, 0
    li      x8, 0
    li      x9, 0
    li      x10,0
    li      x11,0
    li      x12,0
    li      x13,0
    li      x14,0
    li      x15,0
    li      x16,0
    li      x17,0
    li      x18,0
    li      x19,0
    li      x20,0
    li      x21,0
    li      x22,0
    li      x23,0
    li      x24,0
    li      x25,0
    li      x26,0
    li      x27,0
    li      x28,0
    li      x29,0
    li      x30,0
    li      x31,0


init_bss:
    /* init bss section */
    la	    a0, __bss_start
    la	    a1, __bss_end-4     /* section end is actually the start of the next section */
    li	    a2, 0x0
  1:
    sw		a2, 0(a0)
    bgeu	a0, a1, 1f
    addi	a0, a0, 4
    j	    1b
  1:

# initialize stack pointer
.option push
.option norelax
    la      sp, __stack_top
.option pop

    jal     main



_exit:
    li      a7, 93
    ecall

  1:
    j       1b
