#include "dynint.h"
#include <stdlib.h>

dynInt initDynInt(int length) {
    dynInt arr;
    arr.size = sizeof(int)*length;
    arr.length = 0;
    arr.items = malloc(arr.size);

    return arr;
};

void pushDynInt(dynInt *arr, int item) {
    if (arr->size < (arr->length+1) * sizeof(int)) {
        arr->items = realloc(arr->items, arr-> length * arr->size);
    }
    arr->items[arr->length] = item;
    arr->length += 1;
};

void delDynInt(dynInt *arr) {
    free(arr->items);
}
