/**
 * Copyright 2022 TotalJustice.
 * SPDX-License-Identifier: Zlib
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <test_framework.h>


// simple test data
static bool test_1(void)
{
    const uint8_t src_data[] = {0x31, 0x32, 0x33, 0x34, 0x35};
    const uint8_t expected_data[] = {0x35, 0x34, 0x33, 0x32, 0x31};
    const uint8_t patch_data[] = {
        0x42, 0x50, 0x53, 0x31, 0x85, 0x85, 0x80, 0x91, 0x35, 0x34, 0x33, 0x32,
        0x31, 0x1c, 0x3a, 0xf5, 0xcb, 0x9f, 0xa0, 0x29, 0x4a, 0x92, 0xb8, 0x4f,
        0x40
    };
    return patch_runner(PatchType_BPS, expected_data, sizeof(expected_data), src_data, sizeof(src_data), patch_data, sizeof(patch_data));
}

// tests that it rejects invalid patch
static bool test_2(void)
{
    const uint8_t src_data[] = {0x31, 0x32, 0x33, 0x34, 0x35};
    const uint8_t expected_data[] = {0x35, 0x34, 0x33, 0x32, 0x31};
    const uint8_t patch_data[] = {
        0x42, 0x50, 0x53, 0x31, 0x85, 0x85, 0x80, 0x91, 0x35, 0x34, 0x33, 0x32,
        0x31, 0x1c, 0x3a, 0xf5, 0xcb, 0x9f, 0xa0, 0x29, 0x4a, 0x92, 0xb8, 0x4f,
        0x41
    };
    return !patch_runner(PatchType_BPS, expected_data, sizeof(expected_data), src_data, sizeof(src_data), patch_data, sizeof(patch_data));
}

// tests that it rejects invalid src
static bool test_3(void)
{
    const uint8_t src_data[] = {0x31, 0x32, 0x33, 0x34, 0x36};
    const uint8_t expected_data[] = {0x35, 0x34, 0x33, 0x32, 0x31};
    const uint8_t patch_data[] = {
        0x42, 0x50, 0x53, 0x31, 0x85, 0x85, 0x80, 0x91, 0x35, 0x34, 0x33, 0x32,
        0x31, 0x1c, 0x3a, 0xf5, 0xcb, 0x9f, 0xa0, 0x29, 0x4a, 0x92, 0xb8, 0x4f,
        0x40
    };
    return !patch_runner(PatchType_BPS, expected_data, sizeof(expected_data), src_data, sizeof(src_data), patch_data, sizeof(patch_data));
}

// tests the growth of data
static bool test_4(void)
{
    const uint8_t src_data[] = {0x31, 0x32, 0x33, 0x34, 0x35};
    const uint8_t expected_data[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    const uint8_t patch_data[] = {
        0x42, 0x50, 0x53, 0x31, 0x85, 0x89, 0x80, 0x90, 0x8d, 0x36, 0x37, 0x38,
        0x39, 0x1c, 0x3a, 0xf5, 0xcb, 0x26, 0x39, 0xf4, 0xcb, 0x3e, 0xc4, 0xfb,
        0x4b
    };
    return patch_runner(PatchType_BPS, expected_data, sizeof(expected_data), src_data, sizeof(src_data), patch_data, sizeof(patch_data));
}

// tests shrinking of data
static bool test_5(void)
{
    const uint8_t src_data[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    const uint8_t expected_data[] = {0x73, 0x6d, 0x6f, 0x6c};
    const uint8_t patch_data[] = {
        0x42, 0x50, 0x53, 0x31, 0x89, 0x84, 0x80, 0x8d, 0x73, 0x6d, 0x6f, 0x6c,
        0x26, 0x39, 0xf4, 0xcb, 0x06, 0xd2, 0xa0, 0xbe, 0x91, 0xe7, 0xae, 0x26
    };
    return patch_runner(PatchType_BPS, expected_data, sizeof(expected_data), src_data, sizeof(src_data), patch_data, sizeof(patch_data));
}

int main(void)
{
    if (!test_1()) { return 1; }
    if (!test_2()) { return 2; }
    if (!test_3()) { return 3; }
    if (!test_4()) { return 4; }
    if (!test_5()) { return 5; }

    return 0;
}