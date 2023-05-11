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
#define MAX_VALUE_LEN	21
#else
typedef unsigned long int		unsigned_value_t;
typedef long int      			signed_value_t;
#define MAX_VALUE_LEN	13
#endif

int upper_case;

// Simply multiplying by -1 produces wrong results with INT_MIN, LONG_INT_MIN and LONG_LONG_INT_MIN as it overflows
#define ABS_FOR_PRINTING(_X)	(unsigned_value_t)(_X > 0 ? _X : -((signed_value_t)_X))

#define GREATER_OF(_X, _Y)	((_X < _Y) ? _Y : _X)		

static char char_map[2][16] = {{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'},
                              {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}};

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
			uint8_t base = 10;

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
					if (flags & FLAGS_LONG_LONG) {
						idx += print_integer(&buffer[idx], (unsigned_value_t)va_arg(va, unsigned long long int), base, flags);
					} else if (flags & FLAGS_LONG) {
						idx += print_integer(&buffer[idx], (unsigned_value_t)va_arg(va, unsigned long int), base, flags);
					} else {
						idx += print_integer(&buffer[idx], (unsigned_value_t)va_arg(va, unsigned int), base, flags);
					}
					break;
				case 'i': /* fall through */
				case 'd': {
				// General case for signed values
					if (flags & FLAGS_LONG_LONG) {
						const long long int value = va_arg(va, long long int);
						flags |= value < 0 ? FLAGS_NEGATIVE : 0;
						idx += print_integer(&buffer[idx], ABS_FOR_PRINTING(value), base, flags);
					} else if (flags & FLAGS_LONG) {
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

#if 0
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#define is_space(c) (c == ' ' || c == '\t' || c == '\n')

typedef enum {
    INT = 0,
    UNSIGNED_INT,
    LONG_LONG_INT,
    UNSIGNED_LONG_LONG_INT,
	HEXADECIMAL,
    FLOAT,
    CHAR,
    STRING
} type_t;

static int match_pattern(const char *buffer, void *value, type_t type) {
    char scratchpad[100];
    int count = 0;

    if (*buffer && buffer) {
        switch(type) {
            case CHAR:
                *(char *)value = *buffer;
                count = 1;
                break;
            case STRING:
                while (*buffer && !is_space(*buffer)) {
                    *(char *)value++ = *buffer++;
                    count++;
                }
            default:
                break;
        }
    }

    return count;
}

static int _vsnscanf(const char *buffer, const char *format, va_list va) {
    unsigned int idx = 0;
    unsigned int match_count = 0;
    unsigned int match_result = 0;

    while (*format && &buffer[idx]) {
        if (is_space(*format)) {
            format++;
            continue;
        }

        if (is_space(buffer[idx])) {
            idx++;
            continue;
        }

        if (*format != '%') {
            if (buffer[idx] != *format && *format != ' ') {
                printf("ERR");
            }
            idx++;
            format++;
            continue;
        } else {
            format++;

            switch(*format) {
                case 'c':
                    match_result = match_pattern(&buffer[idx], va_arg(va, char*), CHAR);
                    if (match_result) {
                        match_count++;
                        idx += match_result;
                    }
                    break;
                case 's':
                    match_result = match_pattern(&buffer[idx], va_arg(va, char*), STRING);
                    if (match_result) {
                        match_count++;
                        idx += match_result;
                    }
                default:
                    break;
            }

            format++;
        }
    }
    return 0;
}

int usscanf(const char *str, const char *format, ...) {
	va_list va;
	va_start(va, format);
	const int ret = _vsnscanf(str, format, va);
	va_end(va);
	return ret;
}

#endif