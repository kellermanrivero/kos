// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

void kmalloc_init();
void *kmalloc(size_t);
void kfree(void*);