# uPrint

uPrint is a small library written in C for your basic text formatting and parsing needs.
No #ifdef soup, no clever algorithms, no complex variables, just basic stuff that you can understand, fix and expand.

## Features

This library is targeted for embedded applications where its common to have custom data I/O ports (UART, I2C, SPI...), so the included functions simply operate on a buffer, that can be received/trasmitted in the most suitable way. The available functions are:

### usprintf

**`int usprintf(char* buffer, const char* format, ...)`**

**Description:**
Prints formatted output to a string.

**Parameters:**
* **`char* buffer`** Buffer where the string will be stored.
* **`const char* format`** String to be printed including format specifiers.
* **`...`** Additional arguments required by the format specifiers.

**Return value:**
Length of the printed string not including the null character `'\0'`.

### uvsnprintf

**`int uvsnprintf(char* buffer, const char* format, va_list va)`**

**Description:**
Prints formatted output to a string. Useful for implementing custom printf-like functions.

**Parameters:**
* **`char* buffer`** Buffer where the string will be stored.
* **`const char* format`** String to be printed including format specifiers.
* **`va_list va`** List arguments required by the format specifiers.

**Return value:**
Length of the printed string not including the null character `'\0'`.

### usscanf
**`int usscanf(const char *str, const char *format, ...)`**

**Description:**
Reads formatted input from a string.

**Parameters:**
* **`char* str`** String containing data to be parsed.
* **`const char* format`** String containg format specifiers to match.
* **`...`** Additional arguments required by the format specifiers.

**Return value:**
Number of retrieved fields. Returns `EOF` in case of failure.

## Format specifiers
In order to keep a small memory footprint the uPrint uses a reduced set of format specifiers:

* **`%d`** Signed integer number (`int`)
* **`%ld`** Long signed integer number (`long int`)[^1]
* **`lld`** Long long signed integer number  (`long long int`)
* **`%u`** Unsigned integer number (`unsigned int`)
* **`%lu`** Long unsigned integrer number (`unsigned long int`)
* **`%llu`** Long long unsigned integer number (`unsigned long long`)
* **`%x`** Lowercase hexadecimal number
* **`%X`** Uppercase hexadecimal number
* **`%f`** Floating point number (`float`)
* **`%c`** Single character (`char`)
* **`%s`** Null terminated character array (`char*`)

It is possible to specify the precision for floating point numbers (number of decimals) and strings (number of characters).
* **`%.[decimal_places][f,s]`**. The asterisk **`*`** character can be used to specify a variable precision. If precision is not specified or is not valid it defaults to 6. Precisions higher than 7 may produce errors for floating point numbers.

[^1]: In some architectures `long int` is equivalent to `int`.

## Unit testing
This project uses the CPPUTEST testing harness. Execute the `setup_ccputest.sh` to clone the CPPUTEST repository, patch some files, install the dependencies and compile. If there are errors during the installation make sure you have `make`, `g++` and `autoconf`.

Then call `make test` from the root directory to run the tests.