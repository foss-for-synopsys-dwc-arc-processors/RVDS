.globl _semihost_writec

.equ SYSCALL_ADDR, 0x02010000


# int _semihost_writec(int arg)
# Write single character to host' console
# arg is in a0
_semihost_writec:

	addi sp, sp, -8
	sw   a7, 4(sp)
	li   a7, 5				# syscall_id = 5;
	li   t0, SYSCALL_ADDR
	# FIXME: fake mhartid by hardcoding it to 0
	mv   t1, zero
	sw   t1, 0(t0)
	lw   a7, 4(sp)
	addi sp, sp, 8

	ret


# void _semihost_halt(int arg1, int arg2)
# Stop simulation of the test with number in arg1 (a0) with result in arg2 (a1)

.globl _semihost_halt

_semihost_halt:
	addi sp, sp, -8
	sw   a7, 4(sp)
	li   a7, 2				# syscall_id = 0;
	beq  a1, zero, 1f
	li   a7, 3
1:
	li   t0, SYSCALL_ADDR

	# getting hart_id could be a problem, due to lack of permissions 
	# to access mhartid M-level CSR; need to replace it with sys_call()
	
	# csrr t1, mhartid
	li   t1, 0		# hart_id = 0; will work for single core config only

	sw   t1, 0(t0)
	lw   a7, 4(sp)
	addi sp, sp, 8

	ret
	

