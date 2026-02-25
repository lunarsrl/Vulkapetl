#ifndef dynarr
#define dynarr
#include <stdint.h>

struct dynarr {
    char* type;
    uint32_t size;
    uint32_t capacity;
    void* items;

} typedef Dynarr;

Dynarr initDynArr(char* type_str, uint32_t, void* items);
Dynarr pushDynArr(Dynarr* arr, void* item);

#endif
