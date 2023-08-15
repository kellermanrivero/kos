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

char* fdt_prop_get_name(const struct fdt_header *header,
                        const struct fdt_prop_data *prop) {
  void *strings_block = (void *) header;
  strings_block += header->off_dt_strings;
  return strings_block + prop->nameoff;
}

int fdt_prop_of_type(char *name, char** names, size_t len) {
  for(int c = 0; c < len; c++) {
    if (strcmp(name, names[c]) == 0) {
      return 1;
    }
  }
  return 0;
}

fdt_token_t *fdt_prop_u32_print(const struct fdt_header *header,
                                const struct fdt_prop_data *prop,
                                fdt_token_t *cursor) {
  size_t length = prop->len;
  if (length) {
    int32_t *data = (int32_t*) cursor;
    swap_bytes(data, sizeof(int32_t));
    debug_printf("<%d>", *data);
    cursor++;
  }
  return cursor;
}

fdt_token_t *fdt_prop_string_print(const struct fdt_header *header,
                                   const struct fdt_prop_data *prop,
                                   fdt_token_t *cursor) {
  size_t length = prop->len;
  if (length) {
    void *data = cursor;
    debug_printf("\"%s\"", data);
    cursor = data + length;
  }
  return cursor;
}

fdt_token_t *fdt_prop_stringlist_print(const struct fdt_header *header,
                                       const struct fdt_prop_data *prop,
                                       fdt_token_t *cursor) {
  size_t length = prop->len;
  if (length) {
    void *data = cursor;
    void *data_end = data + length;
    while(data < data_end) {
      data = fdt_prop_string_print(header, prop, cursor);
      debug_printf(", ");
    }
    cursor = (fdt_token_t *) data;
  }
  return cursor;
}

fdt_token_t *fdt_prop_generic_print(const struct fdt_header *header,
                                    const struct fdt_prop_data *prop,
                                    fdt_token_t *cursor) {
  size_t length = prop->len;
  if (length > 0) {
    uint32_t offset = 0;
    void *data = cursor;
    while (offset < length) {
      char *c = data + offset++;
      debug_printf("%x ", *c);
    }
    cursor = data + offset;
  }
  return cursor;
}

void dump_device_tree(struct fdt_header *header) {
  void *block_start = ((void *) header) + header->off_dt_struct;
  void *block_end = block_start + header->size_dt_struct;
  debug_msg("Structure block range %p - %p", block_start, block_end);

  int32_t nested_levels = 0;
  fdt_token_t *cursor = block_start;
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
        debug_msg("\b\b}");
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

        char *name = fdt_prop_get_name(header, property);
        if (property->len == 0) {
          debug_msg("%s", name);
        } else {
          debug_printf("%s = ", name);
          if (fdt_prop_of_type(name, FDT_STRING_PROPERTIES, sizeof(FDT_STRING_PROPERTIES))) {
            cursor = fdt_prop_string_print(header, property, cursor);
          } else if (fdt_prop_of_type(name, FDT_STRINGLIST_PROPERTIES, sizeof(FDT_STRINGLIST_PROPERTIES))) {
            cursor = fdt_prop_stringlist_print(header, property, cursor);
          } else if (fdt_prop_of_type(name, FDT_U32_PROPERTIES, sizeof(FDT_U32_PROPERTIES))) {
            cursor = fdt_prop_u32_print(header, property, cursor);
          } else {
            cursor = fdt_prop_generic_print(header, property, cursor);
          }
          debug_msg("");
        }
        cursor = (fdt_token_t *) fdt_align((uintptr_t) cursor, sizeof(fdt_token_t));
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