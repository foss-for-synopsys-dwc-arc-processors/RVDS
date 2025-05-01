/// @file       arch.h
/// @brief      RISC-V Virtual Platform architecture specific register access API

#pragma once

#include "arch_regs.h"

// CSR access macro definitions

#define __csrw(__csr__, __src__)  asm volatile ("csrw %0, %1\n" : : "i"(__csr__), "r"(__src__) : )
#define __csrr(__tgt__, __csr__)  asm volatile ("csrr %0, %1\n" : "=r"(__tgt__) : "i"(__csr__) : )

#define __csrrw(__dst__, __csr__, __src__) asm volatile ("csrrw %0, %1, %2" : "=r"(__dst__) : "i"(__csr__), "r"(__src__)  : )

#define __csrs(__csr__, __msk__)  asm volatile ("csrrs zero, %0, %1\n" : : "i"(__csr__), "r"(__msk__) : )
#define __csrc(__csr__, __msk__)  asm volatile ("csrrc zero, %0, %1\n" : : "i"(__csr__), "r"(__msk__) : )

// S-mode iCSR access macro definitions

#define __icsrw(__icsr__, __src__) asm volatile ("csrw %0, %1\n csrw %2, %3\n" : : "i"(CSR_SISELECT), "r"(__icsr__), "i"(CSR_SIREG), "r"(__src__)  : )
#define __icsrr(__dst__, __icsr__) asm volatile ("csrw %1, %2\n csrr %0, %3\n" : "=r"(__dst__) : "i"(CSR_SISELECT), "r"(__icsr__), "i"(CSR_SIREG) : )

// CSR access by address (arch.S)
extern unsigned long __csr_read(unsigned long csr);
extern void __csr_write(unsigned long csr, unsigned long data);

// S-mode iCSR access (arch.S)
extern unsigned long __icsr_read(unsigned long csr);
extern void __icsr_write(unsigned long csr, unsigned long data);
