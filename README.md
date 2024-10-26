# libpatch

simple patcher for ips, ups and bps files! (written in c99).

NOTE: this does not current support creating patches! only applying them

---

## using

add this to your `CMakeLists.txt`

```cmake
include(FetchContent)

FetchContent_Declare(patch
    GIT_REPOSITORY https://github.com/ITotalJustice/libpatch.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(patch)

target_link_libraries(your_exe PRIVATE patch)
```

an example of how it would look in your code.

```c
uint8_t* apply_patch(
    const uint8_t* src_data, size_t src_size,
    const uint8_t* patch_data, size_t patch_size,
    size_t* out_size
) {
    enum PatchType type;
    if (PatchError_OK != patch_get_type(&type, patch_data, patch_size))
    {
        return NULL;
    }

    if (PatchError_OK != patch_get_size(type, out_size, src_size, patch_data, patch_size))
    {
        return NULL;
    }

    uint8_t* out = malloc(out_size);
    if (!out)
    {
        return NULL
    }

    if (PatchError_OK != patch_apply(type, out, out_size, src_data, src_size, patch_data, patch_size))
    {
        free(out);
        return NULL;
    }

    // patch worked, out is now pointing to allocated data.
    // remember to call free when done!
    return out;
}
```

---

## thanks

- ips spec https://zerosoft.zophar.net/ips.php
- ups spec by near https://www.romhacking.net/documents/392/
- bps spec by near https://www.romhacking.net/documents/746/
