/// @file   semihost.h
/// @brief  RISC-V Virtual Platform - host link interface


#pragma once

	// first arg is the test number, fix it to zero for compatibility with exit()
	extern int __attribute__((noreturn)) _semihost_halt(int arg1, int arg2);
	#define exit(__status__)    _semihost_halt(0, __status__)

	extern int _semihost_writec(int arg);
	#define putchar(__ch__)     _semihost_writec((int)(__ch__))
