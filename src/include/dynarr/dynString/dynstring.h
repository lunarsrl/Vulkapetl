#ifndef dynString
#define dynString
#include <stdint.h>

struct dynString {
    uint32_t size;
    uint32_t length;
    char** items;
} typedef dynStr;

dynStr initDynStr(int length);
void pushDynStr(dynStr* arr, char* item);
void delDynStr(dynStr* arr);
#endif
