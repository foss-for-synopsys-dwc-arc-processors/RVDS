
    .section        ".text"
    
    .global         __csr_read
    .global         __csr_write
    .global         __icsr_read
    .global         __icsr_write


# @func     long __csr_read(long csr)
# @brief    read CSR register
#           use with caution!!! self-modified code!!!  
# @args     a0 - CSR address  
# @return   a0 - data  

__csr_read:

    li      t0, 0x02573
    slli    a0, a0, 20
    or      a0, a0, t0
    la      t0, 1f
    sw      a0, 0(t0)
    fence
1:
    csrr    a0, time

    ret


# @func     long __csr_write(long csr, long data)
# @brief    read CSR register
#           use with caution!!! self-modified code!!!  
# @args     a0 - CSR address  
#           a1 - data to write
# @return   none 

__csr_write:

    li      t0, 0x59073
    slli    a0, a0, 20
    or      a0, a0, t0
    la      t0, 1f
    sw      a0, 0(t0)
    fence
1:
    csrw    time, a1          

    ret


# @func     long __icsr_read(long icsr)
# @brief    read iCSR register
# @args     a0 - iCSR address  
# @return   a0 - data 

__icsr_read:

    csrw    siselect, a0
    csrr    a0, sireg

    ret


# @func     long __icsr_write(long icsr, long data)
# @brief    write data to iCSR register
# @args     a0 - iCSR address
#           a1 - data to write  
# @return   none 

__icsr_write:

    csrw   siselect, a0
    csrw   sireg, a1

    ret
