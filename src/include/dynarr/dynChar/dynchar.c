#include "dynchar.h"
#include <stdlib.h>

dynChar initDynInt(int length) {
    dynChar arr;
    arr.size = sizeof(char)*length;
    arr.length = 0;
    arr.items = malloc(arr.size);

    return arr;
};

void pushDynInt(dynChar *arr, int item) {
    if (arr->size < (arr->length+1) * sizeof(int)) {
        arr->items = realloc(arr->items, arr-> length * arr->size);
    }
    arr->items[arr->length] = item;
    arr->length += 1;
};

void delDynInt(dynChar *arr) {
    free(arr->items);
}
