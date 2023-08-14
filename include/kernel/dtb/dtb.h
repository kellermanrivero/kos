// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>

#define ALIGN(ptr) fdt_align(ptr, sizeof(ptr))

#define FDT_HEADER_MAGIC 0xedfe0dd0
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009
#define FDT_PROP_MODEL "model"
#define FDT_PROP_COMPATIBLE "compatible"

static char *FDT_STRING_PROPERTIES[] = {FDT_PROP_MODEL};
static char *FDT_STRINGLIST_PROPERTIES[] = {FDT_PROP_COMPATIBLE};

typedef uint32_t fdt_token_t;

struct fdt_header {
  uint32_t magic;
  uint32_t totalsize;
  uint32_t off_dt_struct;
  uint32_t off_dt_strings;
  uint32_t off_mem_rsvmap;
  uint32_t version;
  uint32_t last_comp_version;
  uint32_t boot_cpuid_phys;
  uint32_t size_dt_strings;
  uint32_t size_dt_struct;
};

struct fdt_reserve_entry {
  uint64_t address;
  uint64_t size;
};

struct fdt_prop_data {
  uint32_t len;
  uint32_t nameoff;
};

uintptr_t fdt_align(uintptr_t ptr, size_t size);
void fdt_header_print(struct fdt_header *header);
char *fdt_prop_get_name(const struct fdt_header *header,
                        const struct fdt_prop_data *prop);
int fdt_prop_of_type(char *name, char **names, size_t len);
fdt_token_t *fdt_prop_string_print(const struct fdt_header *header,
                                   const struct fdt_prop_data *prop,
                                   fdt_token_t *cursor);
fdt_token_t *fdt_prop_stringlist_print(const struct fdt_header *header,
                                       const struct fdt_prop_data *prop,
                                       fdt_token_t *cursor);
fdt_token_t *fdt_prop_generic_print(const struct fdt_header *header,
                                    const struct fdt_prop_data *prop,
                                    fdt_token_t *cursor);
void fdt_reserve_entry_print(struct fdt_reserve_entry *entry);
void dump_device_tree(struct fdt_header *header);
void parse_device_tree(struct fdt_header *header);