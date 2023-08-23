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

fdt_token_t *fdt_advance_cursor(const fdt_token_t *cursor, size_t offset) {
  // Move the cursor
  uintptr_t new_cursor = (uintptr_t) cursor;
  new_cursor += offset;

  // Align the cursor
  return (fdt_token_t *) fdt_align(new_cursor, sizeof(fdt_token_t));
}

void fdt_traverse(struct fdt_header *header, struct fdt_ops *ops, void *data_ptr) {

  if (ops->begin) {
    ops->begin(data_ptr, header);
  }

  uintptr_t header_start = (uintptr_t) header;
  uintptr_t block_start = header_start + header->off_dt_struct;
  uintptr_t block_end = block_start + header->size_dt_struct;
  fdt_token_t *cursor = (fdt_token_t *) block_start;

  while ((uintptr_t) cursor < block_end) {
    if (ops->visit_token) {
      ops->visit_token(data_ptr, header, cursor);
    }

    fdt_token_t token = *cursor;

    switch (token) {
      case FDT_BEGIN_NODE: {
        DTB_DEBUG_LOG("FDT_BEGIN_NODE", 0);
        fdt_token_t *c = cursor;
        const char *name = (char *) ++cursor;

        if (ops->visit_begin_node) {
          ops->visit_begin_node(data_ptr, header, c, name);
        }

        // Move the cursor
        cursor = fdt_advance_cursor(cursor, strlen(name) + 1);
        break;
      }
      case FDT_END_NODE: {
        DTB_DEBUG_LOG("FDT_END_NODE", 0);
        fdt_token_t *c = cursor++;
        if (ops->visit_end_node) {
          ops->visit_end_node(data_ptr, header, c);
        }
        break;
      }
      case FDT_NOP: {
        DTB_DEBUG_LOG("FDT_NOP", 0);
        fdt_token_t *c = cursor++;
        if (ops->visit_nop_node) {
          ops->visit_nop_node(data_ptr, header, c);
          break;
        }
      }
      case FDT_PROP: {
        DTB_DEBUG_LOG("FDT_PROP", 0);
        fdt_token_t *c = cursor;
        struct fdt_prop_data *property = (struct fdt_prop_data *) ++cursor;
        void *property_value = NULL;
        if (property->len) {
          property_value = property+1;
        }

        if (ops->visit_property) {
          ops->visit_property(data_ptr, header, c, property, property_value);
        }

        // Move the cursor
        cursor = fdt_advance_cursor(cursor, sizeof(struct fdt_prop_data) + property->len);
        break;
      }
      case FDT_END: {
        DTB_DEBUG_LOG("FDT_END", 0);
        goto exit;
      }
      default: {
        DTB_DEBUG_LOG("FDT_ERROR", 0);
        goto exit;
      }
    }
  }

exit:
  if (ops->end) {
    ops->end(data_ptr, header);
  }
}

void fdt_reserve_entry_print(struct fdt_reserve_entry *entry) {
  debug_msg("=============== Reserved Memory Block =================");
  debug_msg("Address: %p (size: %p)", entry->address, entry->size);
  debug_msg("=======================================================");
}

char *fdt_prop_get_name(const struct fdt_header *header,
                        const struct fdt_prop_data *prop) {
  void *strings_block = (void *) header;
  strings_block += header->off_dt_strings;
  return strings_block + prop->nameoff;
}

int fdt_prop_of_type(char *name, char **names, size_t len) {
  for (int c = 0; c < len; c++) {
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
    int32_t *data = (int32_t *) cursor;
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

fdt_token_t *fdt_prop_string_list_print(const struct fdt_header *header,
                                        const struct fdt_prop_data *prop,
                                        fdt_token_t *cursor) {
  size_t length = prop->len;
  if (length) {
    void *data = cursor;
    void *data_end = data + length;
    while (data < data_end) {
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

void parse_device_tree(struct fdt_header *header) {
  // Device tree uses big-endian values, so we need to swap bytes
  struct fdt_ops fixup_ops = {
          .begin = fdt_fixup_header_endianness,
          .visit_token = fdt_fixup_token_endianness,
          .visit_property = fdt_fixup_property_endianness};

  fdt_traverse(header, &fixup_ops, NULL);

  // Dump device tree blob
  struct fdt_ops dump_ops = {
          .begin = fdt_dump_header,
          .visit_begin_node = fdt_dump_begin_node,
          .visit_end_node = fdt_dump_end_node,
          .visit_token = fdt_dump_token,
          .visit_property = fdt_dump_property
  };

  fdt_traverse(header, &dump_ops, NULL);

  // Print reserved memory regions
  void *p = header;
  void *reserve_memory_list = (p + header->off_mem_rsvmap);
  struct fdt_reserve_entry *entry = reserve_memory_list;
  while (entry->address) {
    fdt_reserve_entry_print(entry);
    entry++;
  }
}

/* Endianness fixup */
void fdt_fixup_header_endianness(void* data_ptr, struct fdt_header *header) {
  void *p = header;
  size_t header_length = sizeof(struct fdt_header);
  while (header_length) {
    header_length -= sizeof(int32_t);
    swap_bytes(p + header_length, sizeof(int32_t));
  }
}

void fdt_fixup_token_endianness(void* data_ptr, struct fdt_header *header, fdt_token_t *token) {
  swap_bytes(token, sizeof(fdt_token_t));
}

void fdt_fixup_property_endianness(void* data_ptr, struct fdt_header *header, fdt_token_t *token, struct fdt_prop_data *property, void *property_value) {
  swap_bytes(&property->len, sizeof(int32_t));
  swap_bytes(&property->nameoff, sizeof(int32_t));
}

/* Dump */
static uint32_t nested_levels = 0;
void fdt_dump_header(void* data_ptr, struct fdt_header *header) {
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

void fdt_dump_begin_node(void* data_ptr, struct fdt_header *header, fdt_token_t *token, const char* name) {
  nested_levels++;
  if (*name) {
    debug_msg("%s {", name);
  } else {
    debug_msg("/ {");
  }
}

void fdt_dump_end_node(void* data_ptr, struct fdt_header *header, fdt_token_t *token) {
  debug_msg("\b\b}");
  nested_levels--;
}

void fdt_dump_token(void* data_ptr, struct fdt_header *header, fdt_token_t *token) {
  for (int p = 1; p <= nested_levels; p++) {
    int pp = p;
    while (pp) {
      debug_printf(" ");
      pp--;
    }
  }
}

void fdt_dump_property(void* data_ptr, struct fdt_header *header, fdt_token_t *token, struct fdt_prop_data *property, void *property_value) {
  // Move the cursor ahead of property
  char *name = fdt_prop_get_name(header, property);
  if (property->len == 0) {
    debug_msg("%s", name);
  } else {
    debug_printf("%s = ", name);
    if (fdt_prop_of_type(name, FDT_STRING_PROPERTIES, sizeof(FDT_STRING_PROPERTIES))) {
      fdt_prop_string_print(header, property, property_value);
    } else if (fdt_prop_of_type(name, FDT_STRINGLIST_PROPERTIES, sizeof(FDT_STRINGLIST_PROPERTIES))) {
      fdt_prop_string_list_print(header, property, property_value);
    } else if (fdt_prop_of_type(name, FDT_U32_PROPERTIES, sizeof(FDT_U32_PROPERTIES))) {
      fdt_prop_u32_print(header, property, property_value);
    } else {
      fdt_prop_generic_print(header, property, property_value);
    }
    debug_msg("");
  }
}