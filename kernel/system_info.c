// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <kernel/system_info.h>
#include <kernel/klibc/stdlib.h>

#define MEMORY_NODE_NAME "memory"
#define MEMORY_REG_PROPERTY_NAME "reg"

void fdt_sysinfo_begin_node(void* data_ptr, struct fdt_header *header, fdt_token_t *token, const char* name)
{
  struct kern_system_info_dtb_data *data = data_ptr;
  if (strstr(name, MEMORY_NODE_NAME)) {
    data->is_memory_node = 1;
  }
}

void fdt_sysinfo_end_node(void* data_ptr, struct fdt_header *header, fdt_token_t *token) {
  struct kern_system_info_dtb_data *data = data_ptr;
  data->is_memory_node = 0;
}

void fdt_sysinfo_property(void* data_ptr, struct fdt_header *header, fdt_token_t *token, struct fdt_prop_data *property, void* property_value)
{
  struct kern_system_info_dtb_data *data = data_ptr;
  if (data->is_memory_node && property->len > 0) {
    char *name = fdt_prop_get_name(header, property);
    if (strcmp(name, MEMORY_REG_PROPERTY_NAME) == 0) {
      uint64_t *property_encoded_array = property_value;

      swap_bytes(property_encoded_array, sizeof(uint64_t));
      data->info->pa_ram_base_address = *property_encoded_array++;

      swap_bytes(property_encoded_array, sizeof(uint64_t));
      data->info->pa_ram_size = *property_encoded_array;
    }
  }
}

void fetch_sysinfo(struct kern_system_info* info, struct fdt_header* header) {
  struct kern_system_info_dtb_data data;
  memset(&data, 0x00, sizeof(struct kern_system_info_dtb_data));
  data.info = info;

  struct fdt_ops sysinfo_ops = {
    .visit_begin_node = fdt_sysinfo_begin_node,
    .visit_end_node = fdt_sysinfo_end_node,
    .visit_property = fdt_sysinfo_property
  };

  fdt_traverse(header, &sysinfo_ops, &data);
}