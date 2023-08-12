// Copyright (c) 2023., Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <kernel/klibc/stdlib.h>
#include <kernel/kmalloc.h>

void __vos_main() {
  debug_msg("Welcome to VirtuOS!\r\n");
  debug_msg("By %s (%d) \r\n", "Kellerman Rivero", 2023);
  kmalloc_init();
  debug_msg("Allocating memory for temporal string!\r\n");
  char* ptr = (char*) kmalloc(7);
  debug_msg("Temporal string allocated!\r\n");
  ptr[0] = 'K';
  ptr[1] = 'E';
  ptr[2] = 'L';
  ptr[3] = 'L';
  ptr[4] = 'E';
  ptr[5] = 'R';
  ptr[6] = '\0';
  debug_msg("Printing my name!\r\n");
  debug_msg(ptr);
}