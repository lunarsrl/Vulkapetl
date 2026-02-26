#ifndef dynInteger
#define dynInteger
#include <stdint.h>

struct dynInteger {
    uint32_t size;
    uint32_t length;
    int* items;
} typedef dynInt;

dynInt initDynInt(int length);
void pushDynInt(dynInt* arr, int item);
void delDynInt(dynInt* arr);
#endif
