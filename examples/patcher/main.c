// simple example of using the patchers
// ./exe rom.bin patch.ips out.bin

#include "patch.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t* ROM_DATA = {0};
static uint8_t* PATCH_DATA = {0};
static uint8_t* OUT_DATA = {0};

static size_t ROM_SIZE = {0};
static size_t PATCH_SIZE = {0};
static size_t OUT_SIZE = {0};

static int get_patch_type(const char* file_name)
{
    static const char* extentions[] =
    {
        [PatchType_IPS] = ".ips",
        [PatchType_UPS] = ".ups",
    };

    const char* ext = strrchr(file_name, '.');
    if (!ext)
    {
        return -1;
    }

    if (!strcmp(ext, extentions[PatchType_IPS]))
    {
        return PatchType_IPS;
    }

    if (!strcmp(ext, extentions[PatchType_UPS]))
    {
        return PatchType_UPS;
    }

    return -1;
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

    if (ROM_DATA)
    {
        free(ROM_DATA);
        ROM_DATA = NULL;
    }
    if (PATCH_DATA)
    {
        free(PATCH_DATA);
        PATCH_DATA = NULL;
    }
    if (OUT_DATA)
    {
        free(OUT_DATA);
        OUT_DATA = NULL;
    }

    return 1;
}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        return cleanup("missing args: ./exe rom.bin patch.ips out.bin");
    }

    const char* rom_filename = argv[1];
    const char* patch_filename = argv[2];
    const char* out_filename = argv[3];

    ROM_DATA = readfile(rom_filename, &ROM_SIZE);
    PATCH_DATA = readfile(patch_filename, &PATCH_SIZE);

    if (!ROM_DATA || !ROM_SIZE || !PATCH_DATA || !PATCH_SIZE)
    {
        return cleanup("failed to read files");
    }

    const int patch_type = get_patch_type(patch_filename);
    if (patch_type == -1)
    {
        return cleanup("unkown patch type");
    }

    if (PatchError_OK != patch(patch_type, &OUT_DATA, &OUT_SIZE, ROM_DATA, ROM_SIZE, PATCH_DATA, PATCH_SIZE))
    {
        return cleanup("failed to patch file");
    }

    if (!writefile(out_filename, OUT_DATA, OUT_SIZE))
    {
        return cleanup("failed to write patched file");
    }

    printf("patched: %s\n", out_filename);
    cleanup(NULL);

    return 0;
}
