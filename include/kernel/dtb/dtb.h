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

// U32 properties
#define FDT_PROP_PHANDLE "phandle"
#define FDT_PROP_INTERRUPT_PARENT "interrupt-parent"
#define FDT_PROP_INTERRUPT_CELLS "#interrupt-cells"
#define FDT_PROP_ADDRESS_CELLS "#address-cells"
#define FDT_PROP_SIZE_CELLS "#size-cells"
#define FDT_PROP_CLOCK_CELLS "#clock-cells"
#define FDT_PROP_GPIO_CELLS "#gpio-cells"
#define FDT_PROP_BANK_WIDTH "bank-width"
#define FDT_PROP_DEVICE_WIDTH "device-width"

// String properties
#define FDT_PROP_MODEL "model"
#define FDT_PROP_DEVICE_TYPE "device_type"
#define FDT_PROP_STDOUT_PATH "stdout-path"

// String list properties
#define FDT_PROP_COMPATIBLE "compatible"
#define FDT_PROP_CLOCK_NAMES "clock-names"
#define FDT_PROP_CLOCK_OUTPUT_NAMES "clock-output-names"

static char *FDT_U32_PROPERTIES[] = {FDT_PROP_PHANDLE,
                                     FDT_PROP_INTERRUPT_PARENT,
                                     FDT_PROP_INTERRUPT_CELLS,
                                     FDT_PROP_ADDRESS_CELLS,
                                     FDT_PROP_SIZE_CELLS,
                                     FDT_PROP_CLOCK_CELLS,
                                     FDT_PROP_GPIO_CELLS,
                                     FDT_PROP_BANK_WIDTH,
                                     FDT_PROP_DEVICE_WIDTH};

static char *FDT_STRING_PROPERTIES[] = {FDT_PROP_MODEL,
                                        FDT_PROP_DEVICE_TYPE,
                                        FDT_PROP_STDOUT_PATH};

static char *FDT_STRINGLIST_PROPERTIES[] = {FDT_PROP_COMPATIBLE,
                                            FDT_PROP_CLOCK_NAMES,
                                            FDT_PROP_CLOCK_OUTPUT_NAMES};

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

struct fdt_ops {
  void (*begin)(struct fdt_header *header);
  void (*end)(struct fdt_header *header);
  void (*visit_token)(struct fdt_header *header, fdt_token_t *cursor);
  void (*visit_begin_node)(struct fdt_header *header, fdt_token_t *cursor, const char *name);
  void (*visit_end_node)(struct fdt_header *header, fdt_token_t *cursor);
  void (*visit_nop_node)(struct fdt_header *header, fdt_token_t *cursor);
  void (*visit_property)(struct fdt_header *header, fdt_token_t *cursor, struct fdt_prop_data *property, void* property_value);
};

uintptr_t fdt_align(uintptr_t ptr, size_t size);
fdt_token_t *fdt_advance_cursor(const fdt_token_t *ptr, size_t offset);
void fdt_traverse(struct fdt_header *header, struct fdt_ops *ops);
char *fdt_prop_get_name(const struct fdt_header *header,
                        const struct fdt_prop_data *prop);
int fdt_prop_of_type(char *name, char **names, size_t len);
fdt_token_t *fdt_prop_u32_print(const struct fdt_header *header,
                                const struct fdt_prop_data *prop,
                                fdt_token_t *cursor);
fdt_token_t *fdt_prop_string_print(const struct fdt_header *header,
                                   const struct fdt_prop_data *prop,
                                   fdt_token_t *cursor);
fdt_token_t *fdt_prop_string_list_print(const struct fdt_header *header,
                                        const struct fdt_prop_data *prop,
                                        fdt_token_t *cursor);
fdt_token_t *fdt_prop_generic_print(const struct fdt_header *header,
                                    const struct fdt_prop_data *prop,
                                    fdt_token_t *cursor);
void fdt_reserve_entry_print(struct fdt_reserve_entry *entry);
void parse_device_tree(struct fdt_header *header);

/* Endianess Fixup */
void fdt_fixup_header_endianness(struct fdt_header *header);
void fdt_fixup_token_endianness(struct fdt_header *header, fdt_token_t *token);
void fdt_fixup_property_endianness(struct fdt_header *header, fdt_token_t *token, struct fdt_prop_data *property, void* property_value);

/* Dump */
void fdt_dump_header(struct fdt_header *header);
void fdt_dump_begin_node(struct fdt_header *header, fdt_token_t *token, const char* name);
void fdt_dump_end_node(struct fdt_header *header, fdt_token_t *token);
void fdt_dump_token(struct fdt_header *header, fdt_token_t *token);
void fdt_dump_property(struct fdt_header *header, fdt_token_t *token, struct fdt_prop_data *property, void* property_value);
