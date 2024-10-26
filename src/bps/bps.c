/**
 * Copyright 2022 TotalJustice.
 * SPDX-License-Identifier: Zlib
 */

/* SOURCE: https://www.romhacking.net/documents/764/ */
#include "bps.h"
#include "common/common.h"

#define PATCH_HEADER_SIZE 0x4U
/* header + src / dst / meta sizes + command + crc32 */
#define PATCH_MIN_SIZE (PATCH_HEADER_SIZE + 3U + 1U + 12U)

bool bps_verify_header(const uint8_t* patch, size_t patch_size)
{
    if (!patch || patch_size < PATCH_HEADER_SIZE)
    {
        return false;
    }

    if (patch[0] != 'B' || patch[1] != 'P' || patch[2] != 'S' || patch[3] != '1')
    {
        return false;
    }

    return true;
}

bool bps_get_size(
    const uint8_t* patch, size_t patch_size,
    size_t* dst_size, size_t* src_size, size_t* meta_size, size_t* offset
) {
    (void)patch_size; // unused

    /* the offset is after the header */
    size_t offset_local = PATCH_HEADER_SIZE;

    const size_t source_size = vln_read(patch, &offset_local);
    const size_t target_size = vln_read(patch, &offset_local);
    const size_t metadata_size = vln_read(patch, &offset_local);

    if (dst_size)
    {
        *dst_size = target_size;
    }
    if (src_size)
    {
        *src_size = source_size;
    }
    if (meta_size)
    {
        *meta_size = metadata_size;
    }
    if (offset)
    {
        *offset = offset_local;
    }

    return true;
}

/* dst_size: large enough to fit entire output */
bool bps_patch_apply(
    uint8_t* dst, size_t dst_size,
    const uint8_t* src, size_t src_size,
    const uint8_t* patch, size_t patch_size
) {
    if (!bps_verify_header(patch, patch_size))
    {
        return false;
    }

    size_t patch_offset = PATCH_HEADER_SIZE;
    size_t src_relative_offset = 0;
    size_t dst_offset = 0;
    size_t dst_relative_offset = 0;

    size_t source_size = 0;
    size_t target_size = 0;
    size_t metadata_size = 0;

    if (!bps_get_size(patch, patch_size, &target_size, &source_size, &metadata_size, &patch_offset))
    {
        return false;
    }

    if (src_size != source_size)
    {
        return false;
    }

    if (dst_size != target_size)
    {
        return false;
    }

    /* skip over metadata */
    patch_offset += metadata_size;

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

    enum Action
    {
        SourceRead = 0,
        TargetRead = 1,
        SourceCopy = 2,
        TargetCopy = 3,
    };

    while (patch_offset < patch_size)
    {
        const size_t data = vln_read(patch, &patch_offset);
        const enum Action action = data & 3;
        size_t len = (data >> 2) + 1;

        switch (action)
        {
            case SourceRead: {
                while (len--) {
                    dst[dst_offset] = src[dst_offset];
                    dst_offset++;
                }
            } break;

            case TargetRead: {
                while (len--)
                {
                    dst[dst_offset++] = patch[patch_offset++];
                }
            } break;

            case SourceCopy: {
                const size_t data = vln_read(patch, &patch_offset);
                src_relative_offset += (data & 1 ? -1 : +1) * (data >> 1);
                while (len--)
                {
                    dst[dst_offset++] = src[src_relative_offset++];
                }
            } break;

            case TargetCopy: {
                const size_t data = vln_read(patch, &patch_offset);
                dst_relative_offset += (data & 1 ? -1 : +1) * (data >> 1);
                while (len--)
                {
                    dst[dst_offset++] = dst[dst_relative_offset++];
                }
            } break;

        }
    }

    if (dst_crc != patch_crc32(dst, dst_size))
    {
        return false;
    }

    return true;
}
