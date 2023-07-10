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

#ifndef USING_FLOAT
#define USING_FLOAT 0
#endif

#ifndef USING_LONG_LONG
#define USING_LONG_LONG 0
#endif

#if USING_LONG_LONG
typedef unsigned long long int  unsigned_value_t;
typedef long long int           signed_value_t;
#else
typedef unsigned long int       unsigned_value_t;
typedef long int                signed_value_t;
#endif

#define BASE_BINARY    2
#define BASE_OCTAL     8
#define BASE_DECIMAL  10
#define BASE_HEX      16

#define DEFAULT_PRECISION 6

#define CHECK_FLAGS(_FLAG, _BIT)    (_FLAG && _BIT)
#define FLAGS_NEGATIVE  (1U <<  0U)
#define FLAGS_ZEROPAD   (1U <<  1U)
#define FLAGS_LONG      (1U <<  2U)
#define FLAGS_LONG_LONG (1U <<  3U)
#define FLAGS_PRECISION (1U <<  4U)
#define FLAGS_UPPERCASE (1U <<  6U)

// Simply multiplying by -1 produces wrong results with INT_MIN, LONG_INT_MIN and LONG_LONG_INT_MIN as it overflows
#define ABS_FOR_PRINTING(_X)    (unsigned_value_t)(_X > 0 ? _X : -((signed_value_t)_X))

#define GREATER_OF(_X, _Y)  ((_X < _Y) ? _Y : _X)

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

    *value = 0; // Clear the value for the following operations

    switch (base) {
        case BASE_BINARY:
            while (is_binary(**buffer)) {
                *value = ((*value) << 1U) + (unsigned_value_t)(*((*buffer)++) - '0');
            }
            break;
        case BASE_OCTAL:
            while (is_octal(**buffer)) {
                *value = ((*value) << 3U) + (unsigned_value_t)(*((*buffer)++) - '0');
            }
            break;
        case BASE_DECIMAL:
            while (is_decimal(**buffer)) {
                *value = (*value) * 10U + (unsigned_value_t)(*((*buffer)++) - '0');
            }
            break;
        case BASE_HEX:
            while (is_hex(**buffer)) {
                *value = ((*value) << 4U) + (unsigned_value_t)char_to_hex(**buffer);
                (*buffer)++;
            }
            break;
        default:
            return -1;
    }

    return 0;
}

#if USING_FLOAT
static int string_to_float(const char **buffer, float *value) {
    // Get the integral part
    unsigned_value_t integral_part;
    
    if (string_to_uint(buffer, &integral_part, BASE_DECIMAL) == -1){
        return -1;
    }

    *value = (float)integral_part;

    // Test if there are decimals
    if (**buffer != '.') {
        return 0; // No decimals
    }

    (*buffer)++;

    // Get the fractional part
    unsigned_value_t fractional_part;

    if (string_to_uint(buffer, &fractional_part, BASE_DECIMAL) == 0) {
        float decimals = (float)fractional_part;

        // Keep dividing by 10 until the integral part becomes zero
        while((int)decimals) {
            decimals /= 10.0F;
        }
        
        *value += decimals;
    }

    return 0;
}
#endif

static int _vsnscanf(const char *buffer, const char *format, va_list va) {
    if (buffer == NULL || format == NULL) return 0;
    int match_count = 0;

    while (*format && *buffer) {
        // int error = 0;

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
                        unsigned_value_t value;
                        if (string_to_uint(&buffer, &value, base) == 0) {
                            *(va_arg(va, long long unsigned int *)) = (long long unsigned int)value;
                            match_count++;
                        }
                    } else 
#endif                  
                    if (flags & FLAGS_LONG) {
                        unsigned_value_t value;
                        if (string_to_uint(&buffer, &value, base) == 0) {
                            *(va_arg(va, long unsigned int *)) = (long unsigned int)value;
                            match_count++;
                        }
                    } else {
                        unsigned_value_t value;
                        if (string_to_uint(&buffer, &value, base) == 0) {
                            *(va_arg(va, unsigned int *)) = (unsigned int)value;
                            match_count++;
                        }
                    }
                    break;
                }
                case 'i': /* fall through */
                case 'd': {
                // General case for signed values

                    // Evaluate minus sign
                    if (*buffer == '-') {
                        flags |= FLAGS_NEGATIVE;
                        buffer++;
                    }
#if USING_LONG_LONG
                    if (flags & FLAGS_LONG_LONG) {
                        unsigned_value_t value;
                        if (string_to_uint(&buffer, &value, base) == 0) {
                            *(va_arg(va, long long int *)) = flags & FLAGS_NEGATIVE ? -(long long int)value : (long long int)value;
                            match_count++;
                        }
                    } else 
#endif                  
                    if (flags & FLAGS_LONG) {
                        unsigned_value_t value;
                        if (string_to_uint(&buffer, &value, base) == 0) {
                            *(va_arg(va, long int *)) = flags & FLAGS_NEGATIVE ? -(long int)value : (long int)value;
                            match_count++;
                        }
                    } else {
                        unsigned_value_t value;
                        if (string_to_uint(&buffer, &value, base) == 0) {
                            *(va_arg(va, int *)) = flags & FLAGS_NEGATIVE ? -(int)value : (int)value;
                            match_count++;
                        }
                    }
                    break;
                }
#if USING_FLOAT
                case 'f': /* fall through */
                case 'F': {
                    float value;
                    if (string_to_float(&buffer, &value) == 0) {
                        *(va_arg(va, float *)) = value;
                        match_count++;
                    }
                    break;
                }
#endif
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

static int print_integer(char *buffer, unsigned_value_t value, uint8_t base, uint32_t flags) {
    int length = 0;

    if (flags & FLAGS_NEGATIVE) {
        buffer[length++] = '-';
    }

    // Store the value in reverse order
    char scratch_pad[25] = {0};
    int scratch_len = 0;
    
    do {
        unsigned_value_t digit = value % (unsigned_value_t)base;
        if (digit <= 9) { // digit cannot be negative so no need to test if greater than or equal to zero
            scratch_pad[scratch_len++] = (char)(digit + '0');
        } else if (digit <= 0x0F) { // if we are here we know that the digit is greater than or equal to ten
            scratch_pad[scratch_len++] = (char)(digit + ((flags & FLAGS_UPPERCASE) ? 'A' : 'a'));
        } else { // if bigger than 0x0F then it is an error
            scratch_len = 0;
            length = 0;
            break;
        }
        value /= (unsigned_value_t)base;
    } while (value);

    // Copy the value reversign the order
    while (scratch_len) {
        buffer[length++] = scratch_pad[--scratch_len]; // The order off the ++ and -- operators is important here
    }

    buffer[length] = '\0';
    
    return length;
}

#if USING_FLOAT
static int print_float (float value, char *str, unsigned int precision) {
    int integral_part = (int)value;
    float fractional_part = value;

    int len = print_integer(str, ABS_FOR_PRINTING(integral_part), 10, value < 0 ? FLAGS_NEGATIVE : 0);

    str[len++] = '.';

    while (precision--) {
        integral_part *= 10;
        fractional_part *= 10;
    }

    // We need this extra step because multiplying by -1 while the value is float can cause errors
    int converted_fractional_part = (int)fractional_part - integral_part;

    len += print_integer(&str[len], ABS_FOR_PRINTING(converted_fractional_part), 10, 0);

    return len;
}
#endif

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

#if USING_FLOAT // Not the best, but for now precision is only used for Float and if it is disabled we get a "unused-but-set-variable" warning 
            // Evaluate precision
            unsigned int precision = DEFAULT_PRECISION;
#endif

            if (*format == '.') {
                format++;
#if USING_FLOAT
                unsigned_value_t temp;
                if (string_to_uint(&format, &temp, BASE_DECIMAL) == 0) {
                    precision = (unsigned int)temp;
                }
#endif
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
#if USING_FLOAT
                case 'f': /* fall through */
                case 'F':
                    idx += print_float((float)va_arg(va, double), &buffer[idx], precision);
                    break;
#endif
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
