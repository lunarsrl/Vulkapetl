#ifndef dynCharacter
#define dynCharacter
#include <stdint.h>

struct dynChar{
    uint32_t size;
    uint32_t length;
    int* items;
} typedef dynChar;

dynChar initDynChar(int length);
void pushDynChar(dynChar* arr, int item);
void delDynChar(dynChar* arr);
#endif
