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
};

enum PatchError
{
    PatchError_OK,
    PatchError_HEADER,
    PatchError_BAD_SIZE,
    PatchError_BAD_ARGS,
    PatchError_MALLOC,
    PatchError_PATCH,
};

enum PatchError patch(
    enum PatchType type,
    uint8_t** dst_data, size_t* dst_size,
    const uint8_t* src_data, size_t src_size,
    const uint8_t* patch_data, size_t patch_size
);

#ifdef __cplusplus
}
#endif
