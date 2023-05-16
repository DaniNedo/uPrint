/*
 * uprintf.h
 *
 *  Created on: 2 may. 2023
 *      Author: Dani
 */

#ifndef UPRINTF_H_
#define UPRINTF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#define USING_FLOAT 1
#define USING_LONG_LONG 1

int usprintf(char* buffer, const char* format, ...);

int usscanf(const char *str, const char *format, ...);

#ifdef __cplusplus
}    // extern "C"
#endif

#endif /* UPRINTF_H_ */
