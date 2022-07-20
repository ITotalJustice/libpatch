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
#include <stdbool.h>
#include <patch.h>

bool patch_runner(enum PatchType type, const uint8_t* expected_data, size_t expected_size, const uint8_t* src_data, size_t src_size, const uint8_t* patch_data, size_t patch_size);

#ifdef __cplusplus
}
#endif
