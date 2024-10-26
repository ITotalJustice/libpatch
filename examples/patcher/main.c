/**
 * Copyright 2022 TotalJustice.
 * SPDX-License-Identifier: Zlib
 */

// simple example of using the patchers
// ./exe rom.bin patch.ips out.bin

#include "patch.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t* PATCH_DATA = {0};
static uint8_t* ROM_DATA = {0};
static uint8_t* OUT_DATA = {0};

static size_t PATCH_SIZE = {0};
static size_t ROM_SIZE = {0};
static size_t OUT_SIZE = {0};

struct SizePair
{
    const char* str;
    double size;
};

static const char* PATCH_TYPE_STR[] =
{
    [PatchType_IPS] = "ips",
    [PatchType_UPS] = "ups",
    [PatchType_BPS] = "bps",
};

// not perfect but it'll do
static struct SizePair get_size_pair(size_t size)
{
    struct SizePair pair;
    if (size > 1024 * 1024 * 100)
    {
        pair.str = "GiB";
        pair.size = (double)size / 1024.0 / 1024.0 / 1024.0;
    }
    else if (size > 1024 * 100)
    {
        pair.str = "MiB";
        pair.size = (double)size / 1024.0 / 1024.0;
    }
    else
    {
        pair.str = "KiB";
        pair.size = (double)size / 1024.0;
    }
    return pair;
}

static uint8_t* readfile(const char* filepath, size_t* size)
{
    uint8_t* data = NULL;
    FILE* f = fopen(filepath, "rb");
    if (!f)
    {
        goto fail;
    }

    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    rewind(f);

    if (!*size)
    {
        goto fail;
    }

    data = malloc(*size);
    if (!data)
    {
        goto fail;
    }

    if (*size != fread(data, 1, *size, f))
    {
        goto fail;
    }

    fclose(f);

    return data;

fail:
    if (f)
    {
        fclose(f);
    }
    if (data)
    {
        free(data);
    }
    return NULL;
}

static bool writefile(const char* filepath, const void* data, size_t size)
{
    FILE* f = fopen(filepath, "wb");
    if (!f)
    {
        return false;
    }

    if (size != fwrite(data, 1, size, f))
    {
        goto fail;
    }

    fclose(f);

    return true;

fail:
    if (f)
    {
        fclose(f);
    }
    return false;
}

static int cleanup(const char* error_message)
{
    if (error_message)
    {
        printf("ERROR: %s\n", error_message);
    }

    if (PATCH_DATA)
    {
        free(PATCH_DATA);
    }
    if (ROM_DATA)
    {
        free(ROM_DATA);
    }
    if (OUT_DATA)
    {
        free(OUT_DATA);
    }

    return error_message ? EXIT_FAILURE : EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        return cleanup("missing args: ./exe patch.ips rom.bin out.bin");
    }

    const char* patch_filename = argv[1];
    const char* rom_filename = argv[2];
    const char* out_filename = argv[3];

    PATCH_DATA = readfile(patch_filename, &PATCH_SIZE);
    ROM_DATA = readfile(rom_filename, &ROM_SIZE);

    if (!PATCH_DATA || !PATCH_SIZE || !ROM_DATA || !ROM_SIZE)
    {
        return cleanup("failed to read files");
    }

    enum PatchType type;
    if (PatchError_OK != patch_get_type(&type, PATCH_DATA, PATCH_SIZE))
    {
        return cleanup("unknown patch type");
    }

    if (PatchError_OK != patch_get_size(type, &OUT_SIZE, ROM_SIZE, PATCH_DATA, PATCH_SIZE))
    {
        return cleanup("unknown patch size");
    }

    OUT_DATA = malloc(OUT_SIZE);
    if (!OUT_DATA)
    {
        return cleanup("failed to allocate output");
    }

    if (PatchError_OK != patch_apply(type, OUT_DATA, OUT_SIZE, ROM_DATA, ROM_SIZE, PATCH_DATA, PATCH_SIZE))
    {
        return cleanup("failed to patch file");
    }

    if (!writefile(out_filename, OUT_DATA, OUT_SIZE))
    {
        return cleanup("failed to write patched file");
    }

    printf("patched!\n");
    printf("\tpatch_file: %s\n", patch_filename);
    printf("\tinput_file: %s\n", rom_filename);
    printf("\toutput_file: %s\n", out_filename);
    printf("\tpatch_type: %s\n", PATCH_TYPE_STR[type]);
    printf("\tpatch_size: %.2f %s\n", get_size_pair(PATCH_SIZE).size, get_size_pair(PATCH_SIZE).str);
    printf("\tsrc_size: %.2f %s\n", get_size_pair(ROM_SIZE).size, get_size_pair(ROM_SIZE).str);
    printf("\tdst_size: %.2f %s\n", get_size_pair(OUT_SIZE).size, get_size_pair(OUT_SIZE).str);

    return cleanup(NULL);
}
