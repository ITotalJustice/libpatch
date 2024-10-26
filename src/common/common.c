#include "common.h"

size_t vln_read(const uint8_t* data, size_t* offset)
{
    size_t result = 0;
    size_t shift = 0;

    /* just in case its a bad patch, only run until max size */
    for (size_t i = 0; i < sizeof(size_t); ++i)
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

uint32_t safe_read(const uint8_t* data, size_t* offset, size_t len, size_t size)
{
    uint32_t out = 0;
    *offset += len;

    if (*offset > size)
    {
        return out;
    }

    for (size_t i = 0; i < len; i++)
    {
        out <<= 8;
        out |= data[*offset - len + i];
    }

    return out;
}

void safe_write(uint8_t* data, uint32_t value, size_t* offset, size_t len, size_t size)
{
    *offset += len;

    if (*offset > size)
    {
        return;
    }

    for (size_t i = 0; i < len; i++)
    {
        data[*offset - len + i] = value;
        value >>= 8;
    }
}

uint32_t read32(const uint8_t* data, size_t offset)
{
    return (data[offset + 3] << 24) | (data[offset + 2] << 16) | (data[offset + 1] << 8) | (data[offset + 0]);
}

/* SOURCE: https://web.archive.org/web/20190108202303/http://www.hackersdelight.org/hdcodetxt/crc.c.txt */
uint32_t patch_crc32(const void* data, size_t size)
{
    int j;
    unsigned int i, byte, crc, mask;
    const unsigned char* message = data;

    i = 0;
    crc = 0xFFFFFFFF;
    while (i < size) {
        byte = message[i];            // Get next byte.
        crc = crc ^ byte;
        for (j = 7; j >= 0; j--) {    // Do eight times.
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
        i = i + 1;
    }
    return ~crc;
}
