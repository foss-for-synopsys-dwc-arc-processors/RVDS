### @file   mtwr3.S
### @brief  RISC-V Test Monitor - M-mode nested vectored mode trap wrappers (mtvec.MODE = 3), 
###         Smtsp extension disabled, nested external interrupts are supported,
###         chained external interrupts are supported. 

.global     _m_exc_wrapper
.global     _m_nvi_wrapper
.global     _m_chi_wrapper

.extern     m_trap_vector
.extern     m_nvi_default

.section    ".text"


#
# M-mode exception trap wrapper for nested vectored 
# interrupt handling mode (mtvec.MODE = 3)
#

_m_exc_wrapper:
    
    addi    sp, sp, -(4 * 32)

    sw      ra,  0 * 4(sp)
    sw      t0,  1 * 4(sp)

    
    csrr    t0, mstatus
    sw      t0, 16 * 4(sp)
    csrr    t0, mepc
    sw      t0, 17 * 4(sp)
    csrr    t0, mstatush
    sw      t0, 19 * 4(sp)
    csrr    t0, mcause
    sw      t0, 18 * 4(sp)


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

    # transform cause value to index in trap vector table

    slli    t0, t0, 2       # t0 = mcause << 2 - convert to vector offset
                            # shift .I flag out (must not be set there)

    la      t1, m_trap_vector   
    add     t0, t0, t1      # t0 += t1          - &m_trap_vector[mcause]
    lw      t0, 0(t0)       # t0 = address of M-mode exception trap handler

    addi    a0, sp, 0       # pass pointer to trap stack frame as an argument

    jalr    t0              # call m_trap_vector[mcause]()       

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
    csrw    mstatus, t0
    lw      t0, 17 * 4(sp)
    csrw    mepc, t0                            
    lw      t0, 19 * 4(sp)
    csrw    mstatush, t0
    # do not restore mcause (sp[18])


    lw      t0,  1 * 4(sp)
    lw      ra,  0 * 4(sp)

    addi    sp, sp, (4 * 32) 

    mret


# M-mode nested vectored external (IMSIC) interrupt wrapper for 
# nested vectored interrupt handling mode (mtvec,MODE = 3)

_m_nvi_wrapper:


    addi    sp, sp, -(4 * 32)

    sw      ra,  0 * 4(sp)
    sw      t0,  1 * 4(sp)

    
    csrr    t0, mstatus
    sw      t0, 16 * 4(sp)
    csrr    t0, mepc
    sw      t0, 17 * 4(sp)
    csrr    t0, mstatush        
    sw      t0, 19 * 4(sp)
    
    # preserve mcause and mtopi for test purposes, 
    # not a part of interrupt trap context when .MODE=3
    
    csrr    t0, mcause          
    sw      t0, 18 * 4(sp)
    csrr    t0, mtopi           
    sw      t0, 20 * 4(sp)

    # standard portion of C-ABI general purpose registers    

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

    # pass pointer to stack frame
    addi    a0, sp, 0       

    # call M-mode nested vectored interrupt trap handler
    # there the universal test trap handler is used

    la      t0, m_nvi_default   
    jalr    t0              

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
    csrw    mstatus, t0
    lw      t0, 17 * 4(sp)
    csrw    mepc, t0                            
    lw      t0, 19 * 4(sp)
    csrw    mstatush, t0

    # do not restore mcause (sp[18]) and mtopi (sp[20])


    lw      t0,  1 * 4(sp)
    lw      ra,  0 * 4(sp)

    addi    sp, sp, (4 * 32) 

    mret




# M-mode chained external (IMSIC) interrupt wrapper for
# nested vectored interrupt handling mode (mtvec.MODE = 3)

_m_chi_wrapper:

    mret
