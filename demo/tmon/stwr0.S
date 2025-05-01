### @file   stwr0.S
### @brief  RISC-V Virtual Platform - S-mode direct mode trap wrapper, 
###         Smtsp extension disabled, nested traps are not supported. 

.global     _s_trap_wrapper

.extern     s_trap_vector

.section ".text"

_s_trap_wrapper:

    addi    sp, sp, -(4 * 32)

    sw      ra,  0 * 4(sp)
    sw      t0,  1 * 4(sp)

    csrr    t0, sstatus
    sw      t0, 16 * 4(sp)
    csrr    t0, sepc
    sw      t0, 17 * 4(sp)

    sw      t1,  2 * 4(sp)
    sw      t2,  3 * 4(sp)
    sw      a0,  4 * 4(sp)
    sw      a1,  5 * 4(sp)
    sw      a2,  6 * 4(sp)
    sw      a3,  7 * 4(sp)
    sw      a4,  8 * 4(sp)
    sw      a5,  9 * 4(sp)
    sw      a6, 10 * 4(sp)
    sw      a7, 11 * 4(sp)
    sw      t3, 12 * 4(sp)
    sw      t4, 13 * 4(sp)
    sw      t5, 14 * 4(sp)
    sw      t6, 15 * 4(sp)

    csrr    t0, scause

    # transform cause value to index in trap vector table

    srli    t1, t0, 24      # t1 = scause >> 24 - shift cause id out, move .I flag to bit 7  
    slli    t0, t0, 2       # t0 = scause << 2  - shift .I flag out, get word's index
    add     t0, t0, t1      # t0 += t1          - offset in trap_vector table

    la      t1, s_trap_vector
    add     t0, t0, t1      # t0 += t1          - trap vector address
    lw      t0, 0(t0)       # t0 = S-mode trap vector

    addi    a0, sp, 0       # pass pointer to trap stack frame to handlers

    jalr    t0              # call s_trap_vector[index]()       

    lw      t6, 15 * 4(sp)
    lw      t5, 14 * 4(sp)
    lw      t4, 13 * 4(sp)
    lw      t3, 12 * 4(sp)
    lw      a7, 11 * 4(sp)
    lw      a6, 10 * 4(sp)
    lw      a5,  9 * 4(sp)
    lw      a4,  8 * 4(sp)
    lw      a3,  7 * 4(sp)
    lw      a2,  6 * 4(sp)
    lw      a1,  5 * 4(sp)
    lw      a0,  4 * 4(sp)
    lw      t2,  3 * 4(sp)
    lw      t1,  2 * 4(sp)

    lw      t0, 16 * 4(sp)
    csrw    sstatus, t0
    lw      t0, 17 * 4(sp)
    csrw    sepc, t0
                            # do not restore scause (sp[18])
    lw      t0,  1 * 4(sp)
    lw      ra,  0 * 4(sp)

    addi    sp, sp, (4 * 32) 

    sret
