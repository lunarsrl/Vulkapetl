#include "dynstring.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dynStr initDynStr(int length) {
    dynStr arr;
    arr.size = length * sizeof(char*);
    arr.length = 0;
    arr.items = malloc(arr.size);

    return arr;
};

void pushDynStr(dynStr *arr, char* item) {
    if (arr->size < (arr->length+1) * sizeof(char*)) {
//        printf("Need more memory to fit!\n");
        arr->items = realloc(arr->items, arr->length+1 * sizeof(char*));
        arr->items[arr->length] = item;
        arr->length += 1;
        arr->size = arr->length * sizeof(char*);
    } else {
//        printf("Fits in pre-allocated memory!\n");
        arr->items[arr->length] = item;
        arr->length += 1;
    }

//    printf("Number of items in arr: %i\n", arr->length);
};

void delDynStr(dynStr *arr) {
    free(arr->items);
}
