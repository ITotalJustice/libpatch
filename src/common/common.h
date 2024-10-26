/**
 * Copyright 2022 TotalJustice.
 * SPDX-License-Identifier: Zlib
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdint.h>

/* this can fail if the int is bigger than 8 bytes */
size_t vln_read(const uint8_t* data, size_t* offset);

// reads len number of bytes, maximum of 4.
uint32_t safe_read(const uint8_t* data, size_t* offset, size_t len, size_t size);
// writes len number of bytes, maximum of 4.
void safe_write(uint8_t* data, uint32_t value, size_t* offset, size_t len, size_t size);

// reads a u32.
uint32_t read32(const uint8_t* data, size_t offset);

// crc32, named so that it doesn't clash with zlib.
uint32_t patch_crc32(const void* data, size_t size);

#ifdef __cplusplus
}
#endif
