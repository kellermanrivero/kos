// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <stdarg.h>
#include <kernel/klibc/stdlib.h>

const char *const DecimalDigits = "0123456789";
const char *const HexDigits = "0123456789ABCDEF";

enum BASE {
  DECIMAL = 10,
  HEXADECIMAL = 16
};

volatile unsigned int *const UART0DR = (unsigned int *) 0x09000000;

void debug_putc(char c) {
  *UART0DR = ((unsigned int)c);
}

// Print integer digit by digit
void debug_write_int(int n, enum BASE base) {
  int d;
  while (n) {
    d = n % base;
    n /= base;
    if (base == HEXADECIMAL) {
      debug_putc(HexDigits[d]);
    } else {
      debug_putc(DecimalDigits[d]);
    }
  }
}

void debug_write(const char* string) {
  while(*string) {
    debug_putc(*string++);
  }
}

// Reverse the integer
int reverse(int n) {
  int r = 0, d;
  while (n != 0) {
    d = n % 10;
    r = r * 10 + d;
    n /= 10;
  }
  return r;
}

/**
 * Minimal implementation for debug messages, currently uses PL011
 * @param message the message to be printed
 */
void debug_msg(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  char* s;
  int i, d;
  while (*fmt != '\0') {
    switch (*fmt) {
      case '%':
        switch (*++fmt) {
          // Null ended strings
          case 's':
            s = va_arg(ap, char *);
            debug_write(s);
            break;
          // Integers
          case 'd':
            i = va_arg(ap, int);
            debug_write_int(reverse(i), DECIMAL);
            break;
          case 'p':
            i = va_arg(ap, uintptr_t);
            debug_putc('0');
            debug_putc('x');
            debug_write_int(reverse(i), HEXADECIMAL);
            break;
        }
        break;
      default:
        debug_putc(*fmt);
        break;
    }
    fmt++;
  }

  va_end(ap);
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