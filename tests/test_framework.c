/**
 * Copyright 2022 TotalJustice.
 * SPDX-License-Identifier: Zlib
 */

#include "test_framework.h"
#include <patch.h>
#include <stdlib.h>
#include <string.h>


bool patch_runner(enum PatchType type, const uint8_t* expected_data, size_t expected_size, const uint8_t* src_data, size_t src_size, const uint8_t* patch_data, size_t patch_size)
{
    uint8_t *dst_data = NULL;
    size_t dst_size = 0;

    if (PatchError_OK != patch(type, &dst_data, &dst_size, src_data, src_size, patch_data, patch_size))
    {
        goto fail;
    }

    if (expected_size != dst_size)
    {
        goto fail;
    }

    if (memcmp(expected_data, dst_data, expected_size))
    {
        goto fail;
    }

    if (dst_data)
    {
        free(dst_data);
    }

    return true;

fail:
    if (dst_data)
    {
        free(dst_data);
    }

    return false;
}
