// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <kernel/dtb/dtb.h>

typedef uint64_t pa_address;
typedef uint64_t va_address;

struct kern_system_info {
  pa_address pa_ram_base_address;
  size_t pa_ram_size;
};

struct kern_system_info_dtb_data {
  struct kern_system_info* info;
  uint8_t is_memory_node;
};

/* System Info DTB functions*/
void fdt_sysinfo_begin_node(void* data_ptr, struct fdt_header *header, fdt_token_t *token, const char* name);
void fdt_sysinfo_end_node(void* data_ptr, struct fdt_header *header, fdt_token_t *token);
void fdt_sysinfo_property(void* data_ptr, struct fdt_header *header, fdt_token_t *token, struct fdt_prop_data *property, void* property_value);
void fetch_sysinfo(struct kern_system_info* info, struct fdt_header* header);