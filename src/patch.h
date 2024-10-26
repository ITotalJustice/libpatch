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


enum PatchType
{
    PatchType_IPS,
    PatchType_UPS,
    PatchType_BPS,
};

enum PatchError
{
    PatchError_OK,
    PatchError_HEADER,
    PatchError_BAD_SIZE,
    PatchError_BAD_ARGS,
    PatchError_MALLOC,
    PatchError_PATCH,
    PatchError_BAD_TYPE,
};

enum PatchError patch_get_type(
    enum PatchType* type,
    const uint8_t* patch_data, size_t patch_size
);

enum PatchError patch_get_size(
    enum PatchType type,
    size_t *dst_size, size_t src_size,
    const uint8_t* patch_data, size_t patch_size
);

enum PatchError patch_apply(
    enum PatchType type,
    uint8_t* dst_data, size_t dst_size,
    const uint8_t* src_data, size_t src_size,
    const uint8_t* patch_data, size_t patch_size
);

#ifdef __cplusplus
}
#endif
