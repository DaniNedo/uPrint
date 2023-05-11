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
#include <stddef.h>

int usprintf(char* buffer, const char* format, ...);

#ifdef __cplusplus
}    // extern "C"
#endif

#endif /* UPRINTF_H_ */
