// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/**
 * Writes a byte over UART / Serial interface
 * @param c the byte to be write
 */
void debug_putc(char c);

/**
 * Writes a new line over UART / Serial interface
 * @param fmt A string with format specifiers (similar to printf)
 * @param ... A variadic list of arguments to be printed
 */
void debug_msg(const char* fmt, ...);

/**
 * Prints a debug message over UART / Serial interface
 * @param fmt A string with format specifiers (similar to printf)
 * @param ... A variadic list of arguments to be printed
 */
void debug_printf(const char* fmt, ...);

/**
 * Sets a memory region with a value
 * @param s A pointer to the memory region
 * @param c The value you want to set
 * @param len The length of the region
 * @returns A pointer to the region
 */
void *memset(void *s, int c, size_t len);

/**
 * Swaps bytes in a memory region (useful for changing endianness)
 * @param s A pointer to the memory region
 * @param len The length of the region
 */
void swap_bytes(void* s, size_t len);

/**
 * Compares two strings character by character.
 * @param s1 the first string
 * @param s2 the second string
 * @return If the strings are equal, the function returns 0.
 */
int strcmp(const char* s1, const char* s2);

/**
 * Calculates string length
 * @param s the input string
 * @return the length of the string
 */
size_t strlen(const char* s);