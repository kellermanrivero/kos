// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <kernel/klibc/stdlib.h>
#include <kernel/dtb/dtb.h>

void fdt_header_print(struct fdt_header* header) {
  debug_msg("=============== Device Tree Blob (Header) =================");
  debug_msg("Version: %d (compatible with: %d)", header->version, header->last_comp_version);
  debug_msg("Magic: %x", header->magic);
  debug_msg("Total size: %x", header->totalsize);
  debug_msg("CPU ID: %d", header->boot_cpuid_phys);
  debug_msg("Size of structure block: %x", header->size_dt_struct);
  debug_msg("Offset of structure block: %x", header->off_dt_struct);
  debug_msg("Size of strings block: %x", header->size_dt_strings);
  debug_msg("Offset of strings block: %x", header->off_dt_strings);
  debug_msg("Memory reservation block offset: %x", header->off_mem_rsvmap);
  debug_msg("===========================================================");
}

void parse_device_tree(struct fdt_header* header) {
  // Device tree uses big-endian values, so we need to swap bytes
  void *p = header;
  size_t header_length = sizeof(struct fdt_header);
  while(header_length) {
    header_length -= sizeof(int32_t);
    swap_bytes(p+header_length, sizeof(int32_t));
  }

  fdt_header_print(header);
}