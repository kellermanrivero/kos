// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <limits.h>
#include <stdarg.h>
#include <kernel/klibc/stdlib.h>

const char *const HexDigits = "0123456789ABCDEF";

volatile unsigned int *const UART0DR = (unsigned int *) 0x09000000;

void debug_putc(char c) {
  *UART0DR = ((unsigned int)c);
}

// Print integer digit by digit
void debug_write_int(int64_t n) {
  if (n == 0) {
    debug_putc('0');
    return;
  }

  int64_t d; int64_t r = 0;
  if (n < 0) {
    debug_putc('-');
  }

  // Reverse the number
  while (n) {
    d = n % 10;
    r = r * 10 + d;
    n /= 10;
  }

  // Print reverse number digit by digit
  while (r) {
    d = r % 10;
    r /= 10;
    debug_putc(d + '0');
  }
}

void debug_write_ptr(uintptr_t n, int size) {
  debug_putc('0');
  debug_putc('x');
  int n_shifts = (size  * CHAR_BIT) - 4;
  while(n_shifts >= 0) {
    uintptr_t d = n >> n_shifts;
    if (d) {
      debug_putc(HexDigits[(char) d & 0xF]);
    }
    n_shifts -=4;
  }
}

void debug_write(const char* string) {
  while(*string) {
    debug_putc(*string++);
  }
}

/**
 * Minimal implementation for debug messages, currently uses PL011
 * @param message the message to be printed
 */
void debug_msg(const char* fmt, ...) {
  if (fmt == NULL) {
    return;
  }

  va_list ap;
  va_start(ap, fmt);

  char* s;
  int i; long long l;
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
            debug_write_int(i);
            break;
          case 'x':
            i = va_arg(ap, int);
            debug_write_ptr(i, sizeof(int));
            break;
          case 'p':
            i = va_arg(ap, uintptr_t);
            debug_write_ptr(i, sizeof(uintptr_t));
            break;
          // Long integers
          case 'l':
            l = va_arg(ap, long long);
            debug_write_int(l);
            break;
        }
        break;
      default:
        debug_putc(*fmt);
        break;
    }
    fmt++;
  }

  debug_write("\n\r");
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

void swap_bytes(void* s, size_t len) {
  char *p = s;
  size_t lo, hi;
  for(lo=0, hi=len-1; hi>lo; lo++, hi--)
  {
    char tmp=p[lo];
    p[lo] = p[hi];
    p[hi] = tmp;
  }
}