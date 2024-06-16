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
    // object_props is set somewhere else;
    // SetShaderValueV(shader->shader, shader->object_props,
    //     ...,
    //     SHADER_UNIFORM_VEC3,
    //     5
    // );
}

DynShader add_object(DA *da, Object *obj, size_t selection) {
    DA_push(da, obj);
    return object_map(da, selection, false);
}

void draw_gizmos(Object *obj) {
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
}

// type = 0 - x, 1 - y, 2 - z
#define NEAREST_AXES(type)                                              \
    NearestPointOnLine(pos,                                             \
            Vector3Add(pos, TYPE_OR_ZERO(type, 1.0)),                   \
            ray.position,                                               \
            Vector3Add(ray.position, ray.direction));                   \

#define TOUCH_CONTROL_POS(type) if(                                     \
    GetRayCollisionSphere(ray, Vector3Add(pos, TYPE_OR_ZERO(type, 0.6)  \
                ), .1).hit) {                                           \
                                                                        \
    control.kind = CONTROL_POS_X + type;                                \
                                                                        \
    Vector3 nearest = NEAREST_AXES(type);                               \
                                                                        \
    control.adjustment =                                                \
        (type == 0) * (pos.x - nearest.x) +                             \
        (type == 1) * (pos.y - nearest.y) +                             \
        (type == 2) * (pos.z - nearest.z);                              \
                                                                        \
    break;                                                              \
}

#define TOUCH_CONTROL_SIZE(type) if(                                    \
    GetRayCollisionBox(ray, boundingBox_sized(Vector3Add(pos, (Vector3) \
                {(type==0)*size.x,(type==1)*size.y,(type==2)*size.z}    \
                ), 0.2)).hit) {                                         \
    control.kind = CONTROL_SCALE_X + type;                              \
                                                                        \
    Vector3 nearest = NEAREST_AXES(type);                               \
                                                                        \
    control.adjustment =                                                \
        (type == 0) * (size.x - nearest.x) +                            \
        (type == 1) * (size.y - nearest.y) +                            \
        (type == 2) * (size.z - nearest.z);                             \
                                                                        \
    break;                                                              \
}

struct Control control(Object *obj, Ray ray) {
    // TODO: absolute or zero?
    if(obj->size.x < 0) obj->size.x = 0;
    if(obj->size.y < 0) obj->size.y = 0;
    if(obj->size.z < 0) obj->size.z = 0;

    const Vector3 pos = obj->position;
    const Vector3 size = obj->size;

    struct Control control = {0};

    for(uint8_t i = 0; i < 3; i++) {
        TOUCH_CONTROL_POS(i);
        TOUCH_CONTROL_SIZE(i);
    }

    return control;
}

void apply_manipulation(struct Control *control, Object *obj) {
    switch(control->kind){
        case CONTROL_NONE:
            break;
        case CONTROL_POS_X:
            obj->position.x += control->adjustment;
            break;
        case CONTROL_POS_Y:
            obj->position.y += control->adjustment;
            break;
        case CONTROL_POS_Z:
            obj->position.z += control->adjustment;
            break;
        case CONTROL_SCALE_X:
            obj->size.x += control->adjustment;
            break;
        case CONTROL_SCALE_Y:
            obj->size.y += control->adjustment;
            break;
        case CONTROL_SCALE_Z:
            obj->size.z += control->adjustment;
            break;
        case CONTROL_ANGLE_X:
            obj->rotation.x += control->adjustment;
            break;
        case CONTROL_ANGLE_Y:
            obj->rotation.y += control->adjustment;
            break;
        case CONTROL_ANGLE_Z:
            obj->rotation.z += control->adjustment;
            break;
        case CONTROL_COLOR_R:
            obj->colour.r += control->adjustment;
            break;
        case CONTROL_COLOR_G:
            obj->colour.g += control->adjustment;
            break;
        case CONTROL_COLOR_B:
            obj->colour.b += control->adjustment;
            break;
        case CONTROL_TRANSLATE:
        case CONTROL_ROTATE:
        case CONTROL_SCALE:
        case CONTROL_CORNER_RADIUS:
        case CONTROL_ROTATE_CAMERA:
        case CONTROL_BLOB_AMOUNT:
        default:
            break;
    }
}
