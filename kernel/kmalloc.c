// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <kernel/klibc/stdlib.h>
#include <kernel/kmalloc.h>

#define STARTUP_HEAP_MEMORY (256)

// From: https://moss.cs.iit.edu/cs351/slides/slides-malloc.pdf
#define ALIGNMENT 8 // must be a power of 2
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

__attribute__((section(".heap"))) static int8_t startup_heap_memory[STARTUP_HEAP_MEMORY];

void *find_fit(size_t size) {
  int8_t *heap_start = startup_heap_memory;
  int8_t *heap_end = heap_start+STARTUP_HEAP_MEMORY;
  int8_t *header = heap_start;
  while (header < heap_end) {
    if (!(*header & 1) && *header >= size)
      return header;
    header = (int8_t *) header + (*header & ~1L);
  }
  return NULL;
}

void kmalloc_init() {
  debug_msg("Initializing Heap\r\n");
  memset(startup_heap_memory, 0, sizeof(startup_heap_memory));

  size_t *header = (size_t *) startup_heap_memory;
  *header = STARTUP_HEAP_MEMORY - SIZE_T_SIZE;

  debug_msg("Heap initialized!\r\n");
}

void *kmalloc(size_t size) {
  size_t blk_size = ALIGN(size + SIZE_T_SIZE);
  size_t *header = find_fit(blk_size);
  if (header) {
    *header = *header | 1;
    return header + SIZE_T_SIZE;
  }
  return NULL;
}