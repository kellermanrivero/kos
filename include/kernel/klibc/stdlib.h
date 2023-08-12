// Copyright (c) 2023, Kellerman Rivero <krsloco@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

void debug_msg(const char* fmt, ...);
void *memset(void *s, int c, size_t len);