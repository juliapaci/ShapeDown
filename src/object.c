#include "object.h"
#include "helper.h"

#include <raylib.h>
#include <raymath.h>
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

Control manage_gizmos(Object *obj) {
    const Vector3 pos = obj->position;
    const Vector3 size = obj->size;

    // TODO: axes

    // position
    // x
    DrawLine3D(pos, Vector3Add(pos, (Vector3){0.5,0,0}), RED);
    DrawCylinderEx(
        Vector3Add(pos, (Vector3){0.5,0,0}),
        Vector3Add(pos, (Vector3){.7,0,0}),
        0.1, 0, 12, RED
    );

    // y
    DrawLine3D(pos, Vector3Add(pos, (Vector3){0,0.5,0}), GREEN);
    DrawCylinderEx(
        Vector3Add(obj->position, (Vector3){0,0.5,0}),
        Vector3Add(obj->position, (Vector3){0,0.7,0}),
        0.1, 0, 12, GREEN
    );

    // z
    DrawLine3D(pos, Vector3Add(pos, (Vector3){0,0,0.5}), BLUE);
    DrawCylinderEx(
        Vector3Add(obj->position, (Vector3){0,0,0.5}),
        Vector3Add(obj->position, (Vector3){0,0,0.7}),
        0.1, 0, 12, BLUE
    );

    // sizes
    DrawCube(Vector3Add(pos, (Vector3){size.x,0,0}), .1,.1,.1, RED);
    DrawCube(Vector3Add(pos, (Vector3){0,size.y,0}), .1,.1,.1, GREEN);
    DrawCube(Vector3Add(pos, (Vector3){0,0,size.z}), .1,.1,.1, BLUE);

    return 0;
}
