#include "object.h"

// Dynamic Array
void DA_init(DA *da, size_t size) {
    da->array = malloc(size * sizeof(Object));
    da->capacity = size;
    da->amount = 0;
}

void DA_push(DA *da, Object *obj) {
    if(++da->amount > da->capacity) {
        da->capacity += da->amount;
        da->array = realloc(da->array, da->capacity * sizeof(Object));
    }

    da->array[da->amount] = *obj;
}

void DA_free(DA *da) {
    free(da->array);
    da->array = NULL;
    da->capacity = da->amount = 0;
}
