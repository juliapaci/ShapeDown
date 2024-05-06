#include "object.h"
#include <raylib.h>

void object_map(Object *obj, const char *path) {
    const char *position = TextFormat("point - vec3(%f, %f, %f)", obj->position.x, obj->position.y, obj->position.z);
    const char *size = TextFormat("vec3(%f, %f, %f)", obj->size.x, obj->size.y, obj->size.z);

    const char *append = TextFormat("\tdistance = min(\n%s,\n\t\tdistance\n\t)",
    TextFormat("\t\tsdf_round_box(\n\t\t\t%s,\n\t\t\t%s,\n\t\t\t%f)", position, size, obj->roundedness));

    printf("%s\n", append);
}

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
