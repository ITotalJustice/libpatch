#include "patch.h"
#include "ips/ips.h"
#include "ups/ups.h"
#include <stdlib.h>


enum PatchError patch(
    const enum PatchType type,
    uint8_t** dst_data, size_t* dst_size,
    const uint8_t* src_data, const size_t src_size,
    const uint8_t* patch_data, const size_t patch_size
) {
    if (!dst_data || !dst_size || !src_data || !src_size || !patch_data || !patch_size)
    {
        return PatchError_BAD_ARGS;
    }

    enum PatchError error = PatchError_OK;

    switch (type)
    {
        case PatchType_IPS: {
            if (!ips_verify_header(patch_data, patch_size))
            {
                error = PatchError_HEADER;
                goto fail;
            }

            *dst_size = src_size;
            *dst_data = malloc(*dst_size);
            if (!*dst_data)
            {
                error = PatchError_MALLOC;
                goto fail;
            }

            if (!ips_patch(*dst_data, *dst_size, src_data, src_size, patch_data, patch_size))
            {
                error = PatchError_PATCH;
                goto fail;
            }
        } break;

        case PatchType_UPS: {
            if (!ups_verify_header(patch_data, patch_size))
            {
                error = PatchError_HEADER;
                goto fail;
            }

            // ups patches can increase / decrease the size of src data.
            if (!ups_get_sizes(patch_data, patch_size, dst_size, NULL, NULL))
            {
                error = PatchError_BAD_SIZE;
                goto fail;
            }

            *dst_data = malloc(*dst_size);
            if (!*dst_data)
            {
                error = PatchError_MALLOC;
                goto fail;
            }

            if (!ups_patch(*dst_data, *dst_size, src_data, src_size, patch_data, patch_size))
            {
                error = PatchError_PATCH;
                goto fail;
            }
        } break;
    }

    return PatchError_OK;

fail:
    if (*dst_data)
    {
        free(*dst_data);
        *dst_data = NULL;
    }

    return error;
}
