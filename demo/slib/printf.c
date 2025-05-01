/// @file   printf.c
/// @brief  compact printf() implementation

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#include "arch.h"
#include "semihost.h"

static inline void itoa (unsigned long long int number, unsigned base)
{
	int i = 0;
	unsigned int intermediate = 0;
	unsigned int digits[sizeof(number)*8];

	while (1) {
		digits[i] = number % base;

		if (number < base)
			break;

		number /= base;
		i++;
	}
	i++;

	while (i-- > 0)
	{
		if (digits[i] >= 10)
			intermediate = 'a' - 10;
		else
			intermediate = '0';

		putchar(digits[i] + intermediate);
	}
}

/** @fn _printf_
 * @brief Handles the input stream of characters to print on screen
 * @details Identifies the type of format string, number of arguments and prints the right characer on screen
 * @warning none
 * @param[in] const char
 * @param[Out] No output parameter
 */
void _printf_(const char *fmt, va_list ap)
{
	register const char* p;
	const char* last_fmt;
	register int ch;
	unsigned long long num;
	int base, lflag;
	int backtothebeginning;

	for (;;) {
		for (;(ch = *(unsigned char *) fmt) != '%'; fmt++) {
			if (ch == '\0')
				return;
			putchar(ch);
		}
		fmt++;

		// Process a %-escape sequence
		last_fmt = fmt;
		lflag = 0;

		backtothebeginning = 0;
		for (;;) {

			switch (ch = *(unsigned char *) fmt++) {

				// long flag (doubled for long long)
				case 'l':
					lflag++;
					backtothebeginning = 1;
					break;

				// character
				case 'c':
					putchar(va_arg(ap, int));
					break;

				// string
				case 's':
					if ((p = va_arg(ap, char *)) == NULL)
						p = "(null)";
					for (; (ch = *p) != '\0' ;) {
						putchar(ch);
						p++;
					}
					break;

				// (signed) decimal
				case 'd':
					base = 10;

					if (lflag >= 2)
						num = va_arg(ap, long long);
					else if (lflag ==1)
						num = va_arg(ap, long);
					else
						num = va_arg(ap, int);

					if ((long long) num < 0) {
						putchar('-');
						num = -(long long) num;
					}

					itoa( num, base);

					break;
				// unsigned decimal
				case 'u':
					base = 10;

					if (lflag >= 2)
						num = va_arg(ap, unsigned long long);
					else if (lflag)
						num = va_arg(ap, unsigned long);
					else
						num = va_arg(ap, unsigned int);

					itoa( num, base);

					break;

				// (unsigned) octal
				case 'o':
					// should do something with padding so it's always 3 octits
					base = 8;

					if (lflag >= 2)
						num = va_arg(ap, unsigned long long);
					else if (lflag)
						num = va_arg(ap, unsigned long);
					else
						num = va_arg(ap, unsigned int);

					itoa( num, base);

					break;
                // (unsigned) hexadecimal
				case 'x':
					base = 16;

					if (lflag >= 2)
						num = va_arg(ap, unsigned long long);
					else if (lflag)
						num = va_arg(ap, unsigned long);
					else
						num = va_arg(ap, unsigned int);

					itoa( num, base);

					break;

				// escaped '%' character
				case '%':
					putchar(ch);
					break;

				// unrecognized escape sequence - just print it literally
				default:
					putchar('%');
					fmt = last_fmt;
					break;
			}

			if (backtothebeginning)
			{
				backtothebeginning = 0;
				continue;
			}
			else
				break;
		}
	}
}

/** @fn printf
 * @brief function to print characters on file
 * @details prints the characters on terminal
 * @warning none
 * @param[in] const char*
 * @param[Out] int
 */
int32_t printf(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	_printf_(fmt, ap);

	va_end(ap);
	return 0; // incorrect return value, but who cares, anyway?
}


/// @fn			puts
/// @brief 		platform implementation of puts()
/// @param[in]  s - pointer to string
/// @param[out] int32_t
int32_t puts(const char *s)
{
    while (*s)
    {
        putchar(*s++);
    }
    putchar('\n');
	return 0;
}

