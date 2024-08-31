#pragma once

#include <stddef.h>
#include <stdint.h>

void memmove(void *dst, const void* src, size_t n);

void memcpy(void *dst, const void* src, size_t n);

void memset(void *dst, char c, size_t n);
