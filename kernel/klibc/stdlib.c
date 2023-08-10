// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <kernel/klibc/stdlib.h>

volatile unsigned int *const UART0DR = (unsigned int *) 0x09000000;

/**
 * Minimal implementation for debug messages, currently uses PL011
 * @param message the message to be printed
 */
void debug_msg(const char* message) {
  while (*message != '\0') {
    *UART0DR = ((unsigned int)*message);
    message++;
  }
}

void *memset(void *s, int c, size_t len) {
  uint8_t *dst = s;
  while (len > 0) {
    *dst = (uint8_t) c;
    dst++;
    len--;
  }
  return s;
}