/*
 * uprintf.c
 *
 *  Created on: 2 may. 2023
 *      Author: Dani
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <limits.h>
#include <stdint.h>
#include "uprint.h"

#define OCTAL 8
#define DECIMAL 10
#define HEX 16

#define CHECK_FLAGS(_FLAG, _BIT)	(_FLAG && _BIT)
#define FLAGS_NEGATIVE  (1U <<  0U)
#define FLAGS_ZEROPAD   (1U <<  1U)
#define FLAGS_LONG      (1U <<  2U)
#define FLAGS_LONG_LONG (1U <<  3U)
#define FLAGS_PRECISION (1U <<  4U)
#define FLAGS_UPPERCASE (1U <<  6U)

#define BASE_BINARY    2
#define BASE_OCTAL     8
#define BASE_DECIMAL  10
#define BASE_HEX      16

#ifndef USING_LONG_LONG
#define USING_LONG_LONG 0
#endif

#if USING_LONG_LONG
typedef unsigned long long int	unsigned_value_t;
typedef long long int 			signed_value_t;
#else
typedef unsigned long int		unsigned_value_t;
typedef long int      			signed_value_t;
#endif

int upper_case;

// Simply multiplying by -1 produces wrong results with INT_MIN, LONG_INT_MIN and LONG_LONG_INT_MIN as it overflows
#define ABS_FOR_PRINTING(_X)	(unsigned_value_t)(_X > 0 ? _X : -((signed_value_t)_X))

#define GREATER_OF(_X, _Y)	((_X < _Y) ? _Y : _X)		

<<<<<<< HEAD
#define is_space(c) (c == ' ' || c == '\t' || c == '\n')
#define is_binary(c) (c == '0' || c == '1')
#define is_octal(c) (c >= '0' && c <= '7')
#define is_decimal(c) (c >= '0' && c <= '9')
#define is_hex(c) (is_decimal(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
#define char_to_hex(c) (c <= '9' ? (c) - '0' : ((c <= 'F' ? c - 'A' : (c <= 'f' ? c - 'a' : 0)) + 10))

static int string_to_uint(const char **buffer, unsigned_value_t *value, uint8_t base) {

    // Skip everything until the first digit
    while (1) {
        if (**buffer == '\0') {
            return -1;
        }
        if ((BASE_DECIMAL == base && is_decimal(**buffer)) ||
            (BASE_HEX == base && is_hex(**buffer)) || 
            (BASE_BINARY == base && is_binary(**buffer)) ||
            (BASE_OCTAL == base && is_octal(**buffer))) {
            break;
        }
        (*buffer)++;
    }
=======
static char char_map[2][16] = {{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'},
                              {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}};
>>>>>>> f7a5c036080263e6594bc92bf90b12e724340693

int print_integer(char *buffer, unsigned_value_t value, uint8_t base, uint32_t flags) {
	int length = 0;

	if (flags & FLAGS_NEGATIVE) {
		buffer[length++] = '-';
	}

	// Store the value in reverse order
	int letter_case = (flags & FLAGS_UPPERCASE) ? 1 : 0;
	char scratch_pad[25] = {0};
	int scratch_len = 0;
	
	while (value) {
		scratch_pad[scratch_len++] = char_map[letter_case][value % (unsigned_value_t)base];
		value /= (unsigned_value_t)base;
	}

	// Copy the value reversign the order
	while (scratch_len) {
		buffer[length++] = scratch_pad[--scratch_len]; // The order off the ++ and -- operators is important here
	}

	buffer[length] = '\0';
	
	return length;
}

int print_float (float value, char *str, unsigned int precision) {
    int whole_part = (int)value;
    float decimal_part = value;

    int len = print_integer(str, ABS_FOR_PRINTING(whole_part), 10, value < 0 ? FLAGS_NEGATIVE : 0);

    precision = precision ? precision : 6;

    str[len++] = '.';

    while (precision--) {
		whole_part *= 10;
        decimal_part *= 10;
    }

	// We need this extra step because multiplying by -1 while the value is float can cause errors
	int converted_decimal_part = (int)decimal_part - whole_part;

    len += print_integer(&str[len], ABS_FOR_PRINTING(converted_decimal_part), 10, 0);

    return len;
}

static inline bool is_digit_(char c) {
  return (c >= '0') && (c <= '9');
}

static unsigned int atoi_(const char** str) {
  unsigned int i = 0U;
  while (is_digit_(**str)) {
    i = i * 10U + (unsigned int)(*((*str)++) - '0');
  }
  return i;
}

static int _uvsnprintf(char* buffer, const size_t maxlen, const char* format, va_list va) {
	int idx = 0U;
	(void)(maxlen);
	while(*format) {
		if (*format != '%') {
			buffer[idx++] = *format++;
			continue;
		}
		else {
			format++;

			// Evaluate flags
			unsigned int flags = 0;

			// Evaluate precision
			unsigned int precision = 0;

			if (*format == '.') {
				format++;
				if (is_digit_(*format)) {
					precision = atoi_(&format);
				}
			}

			// Evaluate length
			switch(*format) {
				case 'l':
					flags |= FLAGS_LONG;
					format++;
					if (*format == 'l') {
						flags |= FLAGS_LONG_LONG;
						format++;
					}
					break;
				default:
					break;
			}

			// Evaluate specifier
			const char* p;
			uint8_t base = 10; // The default base is decimal

			switch(*format) {
				case 'X': 
					flags |= FLAGS_UPPERCASE;
					/* fall through */
				case 'x': 
					base = BASE_HEX;
					/* fall through */
				case 'o':
					base = base - BASE_DECIMAL ? base : BASE_OCTAL; // If its the default base change it
					/* fall through */
				case 'b':
					base = base - BASE_DECIMAL ? base : BASE_BINARY; // If its the default base change it
					/* fall through */
				case 'u':
				// General case for unsigned values
#if USING_LONG_LONG
					if (flags & FLAGS_LONG_LONG) {
						idx += print_integer(&buffer[idx], (unsigned_value_t)va_arg(va, unsigned long long int), base, flags);
					} else 
#endif					
					if (flags & FLAGS_LONG) {
						idx += print_integer(&buffer[idx], (unsigned_value_t)va_arg(va, unsigned long int), base, flags);
					} else {
						idx += print_integer(&buffer[idx], (unsigned_value_t)va_arg(va, unsigned int), base, flags);
					}
					break;
				case 'i': /* fall through */
				case 'd': {
				// General case for signed values
#if USING_LONG_LONG
					if (flags & FLAGS_LONG_LONG) {
						const long long int value = va_arg(va, long long int);
						flags |= value < 0 ? FLAGS_NEGATIVE : 0;
						idx += print_integer(&buffer[idx], ABS_FOR_PRINTING(value), base, flags);
					} else
#endif
					if (flags & FLAGS_LONG) {
						const long int value = va_arg(va, long int);
						flags |= value < 0 ? FLAGS_NEGATIVE : 0;
						idx += print_integer(&buffer[idx], ABS_FOR_PRINTING(value), base, flags);
					} else {
						const int value = va_arg(va, int);
						flags |= value < 0 ? FLAGS_NEGATIVE : 0;
						idx += print_integer(&buffer[idx], ABS_FOR_PRINTING(value), base, flags);
					}
					break;
				}
				case 'f': /* fall through */
				case 'F':
					idx += print_float((float)va_arg(va, double), &buffer[idx], precision);
					break;
				case 'c':
					buffer[idx++] = (char)va_arg(va, int);
					break;
				case 's':
					p = va_arg(va, char*);
					if (p == NULL) {
						break;
					}
					while(*p) {
						buffer[idx++] = *p++;
					}
					break;
				case '%':
					buffer[idx++] = '%';
				default:
					break;
			}

			format++;
		}
	}
	buffer[idx] = '\0';
	return idx;
}

int usprintf(char* buffer, const char* format, ...) {
  va_list va;
  va_start(va, format);
  const int ret = _uvsnprintf(buffer, (size_t)-1, format, va);
  va_end(va);
  return ret;
}

#define is_space(c) (c == ' ' || c == '\t' || c == '\n')

#define is_binary(c) (c == '0' || c == '1')
#define is_octal(c) (c >= '0' && c <= '7')
#define is_decimal(c) (c >= '0' && c <= '9')
#define is_hex(c) (is_decimal(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
#define char_to_hex(c) (c <= '9' ? c - '0' : ((c <= 'F' ? c - 'A' : (c <= 'f' ? c - 'a' : 0)) + 10U))

unsigned_value_t string_to_uint(const char **buffer, uint8_t base, int *error) {
	unsigned_value_t value = 0U;
	*error = 0;

	// Skip all everything (including "-") until the first digit
	while (1) {
		if (**buffer == '\0') { *error = -1; return 0; }
		if ((BASE_DECIMAL == base && is_decimal(**buffer)) ||
			(BASE_HEX == base && is_hex(**buffer)) || 
			(BASE_BINARY == base && is_binary(**buffer)) ||
			(BASE_OCTAL == base && is_octal(**buffer))) {
			break;
		}
		(*buffer)++;
	}

	switch (base) {
		case BASE_BINARY:
			while (is_binary(**buffer)) {
				value = (value << 1U) + (unsigned_value_t)(*((*buffer)++) - '0');
			}
			break;
		case BASE_OCTAL:
			while (is_octal(**buffer)) {
				value = (value << 3U) + (unsigned_value_t)(*((*buffer)++) - '0');
			}
			break;
		case BASE_DECIMAL:
			while (is_decimal(**buffer)) {
				value = value * 10U + (unsigned_value_t)(*((*buffer)++) - '0');
			}
			break;
		case BASE_HEX:
			while (is_hex(**buffer)) {
				value = (value << 4U) + (unsigned_value_t)(*((*buffer)++) - '0');// char_to_hex((unsigned_value_t)(*((*buffer)++)));
			}
			break;
		default:
			*error = -1;
			return 0;
	}

	return value;
}

static int _vsnscanf(const char *buffer, const char *format, va_list va) {
	if (buffer == NULL || format == NULL) return 0;
    int match_count = 0;

    while (*format && *buffer) {
		int error = 0;

        if (is_space(*format)) {
            format++;
            continue;
        }

        if (is_space(*buffer)) {
            buffer++;
            continue;
        }

        if (*format != '%') {
            if (*buffer != *format) {
                break;
            }
            buffer++;
            format++;
            continue;
        } else {
			format++;

			unsigned int flags = 0;

			// Evaluate length
			switch(*format) {
				case 'l':
					flags |= FLAGS_LONG;
					format++;
					if (*format == 'l') {
						flags |= FLAGS_LONG_LONG;
						format++;
					}
					break;
				default:
					break;
			}

			uint8_t base = BASE_DECIMAL; // Default base is decimal

            switch(*format) {
				case 'X': 
					flags |= FLAGS_UPPERCASE;
					/* fall through */
				case 'x': 
					base = BASE_HEX;
					/* fall through */
				case 'o':
					base = base - BASE_DECIMAL ? base : BASE_OCTAL; // If its the default base change it
					/* fall through */
				case 'b':
					base = base - BASE_DECIMAL ? base : BASE_BINARY; // If its the default base change it
					/* fall through */
				case 'u': {
					// General case for unsigned values
#if USING_LONG_LONG
					if (flags & FLAGS_LONG_LONG) {
						const long long unsigned int value = (long long unsigned int)string_to_uint(&buffer, base, &error);
						if (error == 0) { *(va_arg(va, long long unsigned int *)) = value; match_count++; }
					} else 
#endif					
					if (flags & FLAGS_LONG) {
						const long unsigned int value = (long unsigned int)string_to_uint(&buffer, base, &error);
						if (error == 0) { *(va_arg(va, long unsigned int *)) = value; match_count++; }
					} else {
						const unsigned int value = (unsigned int)string_to_uint(&buffer, base, &error);
						if (error == 0) { *(va_arg(va, unsigned int *)) = value; match_count++;	}
					}
					break;
				}
				case 'c':
					*(va_arg(va, char *))  = *buffer++;
					match_count++;                
                    break;
                case 's': {
					char *str = va_arg(va, char *);
					if (*buffer && !is_space(*buffer)) {
						match_count++;
					}
					while (*buffer && !is_space(*buffer)) {
						*str++ = *buffer++;
					}
					break;
				}
                default:
                    break;
            }

            format++;
        }
    }
    return match_count;
}

int usscanf(const char *str, const char *format, ...) {
	va_list va;
	va_start(va, format);
	const int ret = _vsnscanf(str, format, va);
	va_end(va);
	return ret;
}
