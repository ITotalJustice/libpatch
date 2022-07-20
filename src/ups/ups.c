/**
 * Copyright 2022 TotalJustice.
 * SPDX-License-Identifier: Zlib
 */

/* SOURCE: https://www.romhacking.net/documents/392/ */
#include "ups.h"
#include <string.h>


#define PATCH_HEADER_SIZE 0x4
/* header + in / out sizes + crc32 */
#define PATCH_MIN_SIZE (PATCH_HEADER_SIZE + 2 + 12)


/* SOURCE: https://web.archive.org/web/20190108202303/http://www.hackersdelight.org/hdcodetxt/crc.c.txt */
static uint32_t crc32(const uint8_t* data, const size_t size)
{
    int crc;
    unsigned int byte, c;
    const unsigned int g0 = 0xEDB88320,    g1 = g0>>1,
        g2 = g0>>2, g3 = g0>>3, g4 = g0>>4, g5 = g0>>5,
        g6 = (g0>>6)^g0, g7 = ((g0>>6)^g0)>>1;

    crc = 0xFFFFFFFF;
    for (size_t i = 0; i < size; i++) {
        byte = data[i];
        crc = crc ^ byte;
        c = ((crc<<31>>31) & g7) ^ ((crc<<30>>31) & g6) ^
            ((crc<<29>>31) & g5) ^ ((crc<<28>>31) & g4) ^
            ((crc<<27>>31) & g3) ^ ((crc<<26>>31) & g2) ^
            ((crc<<25>>31) & g1) ^ ((crc<<24>>31) & g0);
        crc = ((unsigned)crc >> 8) ^ c;
    }
    return ~crc;
}

/* this can fail if the int is bigger than 8 bytes */
static size_t vln_read(const uint8_t* data, size_t* offset)
{
    size_t result = 0;
    size_t shift = 0;

    /* just in case its a bad patch, only run until max size */
    for (uint8_t i = 0; i < sizeof(size_t); ++i)
    {
        const uint8_t value = data[*offset];
        ++*offset;

        if (value & 0x80)
        {
            result += (value & 0x7F) << shift;
            break;
        }

        result += (value | 0x80) << shift;
        shift += 7;
    }

    return result;
}

static uint8_t safe_read(const uint8_t* data, size_t* offset, const size_t size)
{
    if (*offset < size)
    {
        const uint8_t value = data[*offset];
        ++*offset;

        return value;
    }

    return 0;
}

static void safe_write(uint8_t* data, const uint8_t value, size_t* offset, const size_t size)
{
    if (*offset < size)
    {
        data[*offset] = value;
        ++*offset;
    }
}

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

bool ups_get_sizes(
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
bool ups_patch(
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

    if (!ups_get_sizes(patch, patch_size, &output_size, &input_size, &patch_offset))
    {
        return false;
    }

    if (dst_size < output_size)
    {
        return false;
    }

    /* crc's are at the last 12 bytes, each 4 bytes each. */
    uint32_t src_crc = 0;
    uint32_t dst_crc = 0;
    uint32_t patch_crc = 0;

    memcpy(&src_crc, patch + (patch_size - 12), sizeof(src_crc));
    memcpy(&dst_crc, patch + (patch_size - 8), sizeof(dst_crc));
    memcpy(&patch_crc, patch + (patch_size - 4), sizeof(patch_crc));

    /* check that the src and patch is valid. */
    /* dst is checked at the end. */
    if (src_crc != crc32(src, src_size))
    {
        return false;
    }

    /* we don't check it's own crc32 (obviously) */
    if (patch_crc != crc32(patch, patch_size - 4))
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
            const uint8_t value = safe_read(src, &src_offset, src_size);
            safe_write(dst, value, &dst_offset, dst_size);
        }

        while (dst_offset < dst_size)
        {
            const uint8_t patch_value = safe_read(patch, &patch_offset, patch_size);
            const uint8_t src_value = safe_read(src, &src_offset, src_size);

            safe_write(dst, src_value ^ patch_value, &dst_offset, dst_size);

            if (patch_value == 0)
            {
                break;
            }
        }
    }

    /* patch can be smaller than src, in this case keep writing from src */
    while (src_offset < src_size && dst_offset < dst_size)
    {
        const uint8_t value = safe_read(src, &src_offset, src_size);
        safe_write(dst, value, &dst_offset, dst_size);
    }

    if (dst_crc != crc32(dst, dst_size))
    {
        return false;
    }

    return true;
}
