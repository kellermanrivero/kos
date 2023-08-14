// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <kernel/dtb/dtb.h>
#include <kernel/klibc/stdlib.h>

#ifdef DTB_DEBUG
#define DTB_DEBUG_LOG(fmt, args...) debug_msg(fmt, args)
#else
#define DTB_DEBUG_LOG(fmt, args...)
#endif

uintptr_t fdt_align(uintptr_t ptr, size_t size) {
  return ((ptr + (size - 1)) & ~(size - 1));
}

void fdt_header_print(struct fdt_header *header) {
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

void fdt_reserve_entry_print(struct fdt_reserve_entry *entry) {
  debug_msg("=============== Reserved Memory Block =================");
  debug_msg("Address: %p (size: %p)", entry->address, entry->size);
  debug_msg("=======================================================");
}


void dump_device_tree(struct fdt_header *header) {
  void *block_start = ((void *) header) + header->off_dt_struct;
  void *block_end = block_start + header->size_dt_struct;

  void *strings_start = ((void *) header) + header->off_dt_strings;
  debug_msg("Structure block range %p - %p", block_start, block_end);

  int32_t nested_levels = 1;
  int32_t *cursor = block_start;
  while ((void *) cursor < block_end) {
    swap_bytes(cursor, sizeof(int32_t));
    int32_t token = *cursor;
    DTB_DEBUG_LOG("Token at %p, value is %x", cursor, token);

    // Add some spaces to simulate nesting
    for(int p = 1; p <= nested_levels; p++) {
      int pp = p;
      while(pp) {
        debug_printf(" ");
        pp--;
      }
    }

    switch (token) {
      case FDT_BEGIN_NODE: {
        DTB_DEBUG_LOG("FDT_BEGIN_NODE");
        ++nested_levels;
        ++cursor;
        if (*cursor) {
          char *c = (char*) cursor;
          debug_printf("%s {", c);
          while (*c) {
            c++;
          }
          debug_msg("");
          c++;
          cursor = (int32_t *) fdt_align((uintptr_t) c, sizeof(int32_t));
        } else {
          debug_msg("/ {");
          ++cursor;
        }
        break;
      }
      case FDT_END_NODE:
        DTB_DEBUG_LOG("FDT_END_NODE");
        --nested_levels;
        ++cursor;
        debug_msg("}");
        break;
      case FDT_NOP:
        DTB_DEBUG_LOG("FDT_NOP");
        ++cursor;
        break;
      case FDT_PROP:
        DTB_DEBUG_LOG("FDT_PROP");
        ++cursor;

        struct fdt_prop_data *property = (struct fdt_prop_data *) cursor;
        ++cursor;
        ++cursor;

        swap_bytes(&property->len, sizeof(int32_t));
        swap_bytes(&property->nameoff, sizeof(int32_t));

        char *name = strings_start + property->nameoff;
        debug_msg("Property: %s (len = %d)", name, property->len);

        if (property->len > 0) {
          uint32_t offset = 0;
          void *extra_data = cursor;
          while (offset < property->len) {
            char *c = extra_data + offset++;
            debug_printf("%x ", *c);
          }
          debug_msg("");
          cursor = (int32_t *) fdt_align((uintptr_t) extra_data + offset, sizeof(int32_t));
        }
        break;
      case FDT_END:
        DTB_DEBUG_LOG("FDT_END");
        goto exit;
      default:
        DTB_DEBUG_LOG("FDT_ERROR");
        goto exit;
    }
  }

exit:
  // Nothing
  debug_msg("End of structure block");
}

void parse_device_tree(struct fdt_header *header) {
  // Device tree uses big-endian values, so we need to swap bytes
  void *p = header;
  size_t header_length = sizeof(struct fdt_header);
  while (header_length) {
    header_length -= sizeof(int32_t);
    swap_bytes(p + header_length, sizeof(int32_t));
  }

  fdt_header_print(header);

  // Print reserved memory regions
  void *reserve_memory_list = (p + header->off_mem_rsvmap);
  struct fdt_reserve_entry *entry = reserve_memory_list;
  while (entry->address) {
    fdt_reserve_entry_print(entry);
    entry++;
  }

  dump_device_tree(header);
}