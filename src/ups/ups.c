/**
 * Copyright 2022 TotalJustice.
 * SPDX-License-Identifier: Zlib
 */

/* SOURCE: https://www.romhacking.net/documents/392/ */
#include "ups.h"
#include "common/common.h"

#define PATCH_HEADER_SIZE 0x4U
/* header + in / out sizes + crc32 */
#define PATCH_MIN_SIZE (PATCH_HEADER_SIZE + 2U + 12U)

bool ups_verify_header(const uint8_t* patch, const size_t patch_size)
{
    if (!patch || patch_size < PATCH_HEADER_SIZE)
    {
        return false;
    }

    if (patch[0] != 'U' || patch[1] != 'P' || patch[2] != 'S' || patch[3] != '1')
    {
        return false;
    }

    return true;
}

bool ups_get_size(
    const uint8_t* patch, const size_t patch_size,
    size_t* dst_size, size_t* src_size, size_t* offset
) {
    (void)patch_size; // unused

    /* the offset is after the header */
    size_t offset_local = PATCH_HEADER_SIZE;

    const size_t input_size = vln_read(patch, &offset_local);
    const size_t output_size = vln_read(patch, &offset_local);

    if (src_size != NULL)
    {
        *src_size = input_size;
    }
    if (dst_size != NULL)
    {
        *dst_size = output_size;
    }
    if (offset != NULL)
    {
        *offset = offset_local;
    }

    return true;
}

/* applies the ups patch to the dst data */
bool ups_patch_apply(
    uint8_t* dst, const size_t dst_size,
    const uint8_t* src, const size_t src_size,
    const uint8_t* patch, size_t patch_size
) {
    if (!dst || !dst_size || !src || !src_size || !patch || patch_size < PATCH_MIN_SIZE)
    {
        return false;
    }

    size_t patch_offset = 0;
    size_t input_size = 0;
    size_t output_size = 0;

    if (!ups_verify_header(patch, patch_size))
    {
        return false;
    }

    if (!ups_get_size(patch, patch_size, &output_size, &input_size, &patch_offset))
    {
        return false;
    }

    if (dst_size < output_size)
    {
        return false;
    }

    /* crc's are at the last 12 bytes, each 4 bytes each. */
    const uint32_t src_crc = read32(patch, patch_size - 12);
    const uint32_t dst_crc = read32(patch, patch_size - 8);
    const uint32_t patch_crc = read32(patch, patch_size - 4);

    /* check that the src and patch is valid. */
    /* dst is checked at the end. */
    if (src_crc != patch_crc32(src, src_size))
    {
        return false;
    }

    /* we don't check it's own crc32 (obviously) */
    if (patch_crc != patch_crc32(patch, patch_size - 4))
    {
        return false;
    }

    /* we've read the crc's now, reduce the size. */
    patch_size -= 12;

    size_t src_offset = 0;
    size_t dst_offset = 0;


    /* read hunks and patch */
    while (patch_offset < patch_size)
    {
        size_t len = vln_read(patch, &patch_offset);

        while (len-- && dst_offset < dst_size)
        {
            const uint8_t value = safe_read(src, &src_offset, 1, src_size);
            safe_write(dst, value, &dst_offset, 1, dst_size);
        }

        while (dst_offset < dst_size)
        {
            const uint8_t patch_value = safe_read(patch, &patch_offset, 1, patch_size);
            const uint8_t src_value = safe_read(src, &src_offset, 1, src_size);

            safe_write(dst, src_value ^ patch_value, &dst_offset, 1, dst_size);

            if (patch_value == 0)
            {
                break;
            }
        }
    }

    /* patch can be smaller than src, in this case keep writing from src */
    while (src_offset < src_size && dst_offset < dst_size)
    {
        dst[dst_offset++] = src[src_offset++];
    }

    if (dst_crc != patch_crc32(dst, dst_size))
    {
        return false;
    }

    return true;
}
