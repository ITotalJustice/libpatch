/**
 * Copyright 2022 TotalJustice.
 * SPDX-License-Identifier: Zlib
 */

#include "patch.h"
#include "ips/ips.h"
#include "ups/ups.h"
#include "bps/bps.h"

enum PatchError patch_get_type(
    enum PatchType* type,
    const uint8_t* patch_data, size_t patch_size
) {
    #define PATCH_FUNC(func, patch_type) \
        if (func(patch_data, patch_size)) { \
            *type = patch_type; \
            return PatchError_OK; \
        }

    PATCH_FUNC(ips_verify_header, PatchType_IPS);
    PATCH_FUNC(ups_verify_header, PatchType_UPS);
    PATCH_FUNC(bps_verify_header, PatchType_BPS);

    #undef PATCH_FUNC
    return PatchError_BAD_TYPE;
}

enum PatchError patch_get_size(
    enum PatchType type,
    size_t* dst_size, size_t src_size,
    const uint8_t* patch_data, size_t patch_size
) {
    #define PATCH_FUNC(func) \
        if (!func) { \
            return PatchError_PATCH; \
        } \
        return PatchError_OK;

    switch (type) {
        case PatchType_IPS: PATCH_FUNC(ips_get_size(patch_data, patch_size, src_size, dst_size));
        case PatchType_UPS: PATCH_FUNC(ups_get_size(patch_data, patch_size, dst_size, NULL, NULL));
        case PatchType_BPS: PATCH_FUNC(bps_get_size(patch_data, patch_size, dst_size, NULL, NULL, NULL));
    }

    #undef PATCH_FUNC
    return PatchError_BAD_TYPE;
}

enum PatchError patch_apply(
    enum PatchType type,
    uint8_t* dst_data, size_t dst_size,
    const uint8_t* src_data, size_t src_size,
    const uint8_t* patch_data, size_t patch_size
) {
    #define PATCH_FUNC(func) \
        if (!func(dst_data, dst_size, src_data, src_size, patch_data, patch_size)) { \
            return PatchError_PATCH; \
        } \
        return PatchError_OK;

    switch (type) {
        case PatchType_IPS: PATCH_FUNC(ips_patch_apply);
        case PatchType_UPS: PATCH_FUNC(ups_patch_apply);
        case PatchType_BPS: PATCH_FUNC(bps_patch_apply);
    }

    #undef PATCH_FUNC
    return PatchError_BAD_TYPE;
}
