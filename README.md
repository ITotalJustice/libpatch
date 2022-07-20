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
    const uint8_t* patch_data, size_t patch_size, enum PatchType type,
    size_t* out_size
) {

    uint8_t* out = NULL;

    const enum PatchError result = patch(
        type,
        &out, out_size,
        src_data, src_size,
        patch_data, patch_size
    );

    // if patch failed, no data is allocated, so out==NULL
    if (result != PatchError_OK)
    {
        // handle error here
        return NULL;
    }
    else
    {
        // patch worked, out is now pointing to allocated data.
        // remember to call free when done!
        return out;
    }
}
```

---

## thanks

- ips spec https://zerosoft.zophar.net/ips.php
- ups spec by near https://www.romhacking.net/documents/392/
- bps spec by near https://www.romhacking.net/documents/746/
