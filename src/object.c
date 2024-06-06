#include "object.h"
#include "helper.h"

#include <raylib.h>
#include <stdio.h>
#include <string.h>

void DA_init(DA *da, size_t size) {
    da->array = malloc(size * sizeof(Object));
    da->capacity = size;
    da->amount = 0;
}

void DA_push(DA *da, Object *obj) {
    if(++da->amount >= da->capacity) {
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

void update_shader_uniforms(DynShader *shader, Camera *camera) {
    SetShaderValue(shader->shader, shader->resolution,
        (float[2]){(float)GetScreenWidth(), (float)GetScreenHeight()},
        SHADER_UNIFORM_VEC2
    );
    SetShaderValue(shader->shader, shader->view_eye,
        &camera->position,
        SHADER_UNIFORM_VEC3
    );
    SetShaderValue(shader->shader, shader->view_center,
        &camera->target,
        SHADER_UNIFORM_VEC3
    );
    SetShaderValue(shader->shader, shader->object_props,
        &camera->target,
        SHADER_UNIFORM_VEC3
    );
}

DynShader add_object(DA *da, Object *obj) {
    DA_push(da, obj);
    return object_map(da, NO_SELECTION);
}

void draw_gizmos(Object *obj) {

}
