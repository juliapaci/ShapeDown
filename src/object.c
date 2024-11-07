#include "object.h"
#include "helper.h"

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

Object DEFAULT_OBJECT = {.size = (Vector3){1.0, 1.0, 1.0}, .colour = {150, 150, 150}, 0};

void DA_init(DA *da, uint16_t size) {
    da->array = malloc(size * sizeof(Object));
    da->capacity = size;
    da->amount = 0;
}

void DA_push(DA *da, Object *obj) {
    if(da->amount >= 255*3)
        return;

    if(da->amount >= da->capacity) {
        da->capacity += da->amount;
        da->array = realloc(da->array, da->capacity * sizeof(Object));
    }

    da->array[da->amount++] = *obj;
}

void DA_remove(DA *da, uint16_t index) {
    da->amount--;
    for(int i = index; i < da->amount; i++) {
        da->array[index] = da->array[index + 1];
    }
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

    // object_props is set somewhere else
}

#define DRAW_POS_CONTROL(axis, pos)                             \
    DrawLine3D(pos, Vector3Add(pos, TYPE_OR_ZERO(axis, 0.5)),   \
            TYPE_OR_ZERO_COLOUR(axis));                         \
    DrawCylinderEx(                                             \
        Vector3Add(pos, TYPE_OR_ZERO(axis, 0.5)),               \
        Vector3Add(pos, TYPE_OR_ZERO(axis, 0.7)),               \
        0.1, 0, 12, TYPE_OR_ZERO_COLOUR(axis))

#define DRAW_SIZE_CONTROL(axis, pos, size) DrawCube(Vector3Add(pos, TYPE_OR_ZERO_VEC3(axis, size)), .1,.1,.1, TYPE_OR_ZERO_COLOUR(axis))

#define DRAW_ROT_CONTROL(axis, pos) DrawCircle3D(pos, RING_RADIUS, (Vector3){1.0 * (axis == Z), 1.0 * (axis == Y), 0.0}, (axis != X) * 90.0, TYPE_OR_ZERO_COLOUR(axis))

void draw_gizmos(Object *obj, ControlKind control) {
    const Vector3 pos = obj->position;
    const Vector3 size = obj->size;

    // TODO: axes

    if(control == CONTROL_NONE)
        for(uint8_t axis = 0; axis < 3; axis++) {
            DRAW_POS_CONTROL(axis, pos);
            DRAW_SIZE_CONTROL(axis, pos, size);
            DRAW_ROT_CONTROL(axis,  pos);
        }
    else
        for(uint8_t axis = 0; axis < 3; axis++) {
            if(false) {}
            else if(control == CONTROL_POS_X + (ControlKind)axis) {DRAW_POS_CONTROL(axis, pos);}
            else if(control == CONTROL_SCALE_X + (ControlKind)axis) {DRAW_SIZE_CONTROL(axis, pos, size);}
            else if(control == CONTROL_ANGLE_X + (ControlKind)axis) {DRAW_ROT_CONTROL(axis, pos);}
        }
}

// type = `Axis` (0 - x, 1 - y, 2 - z)
#define NEAREST_POINT_LINE(axis)                                        \
    NearestPointOnLine(pos,                                             \
            Vector3Add(pos, TYPE_OR_ZERO(axis, 1.0)),                   \
            ray.position,                                               \
            Vector3Add(ray.position, ray.direction));                   \

#define TOUCH_CONTROL_POS(axis) else if(                                \
    GetRayCollisionSphere(ray, Vector3Add(pos, TYPE_OR_ZERO(axis, 0.6)  \
                ), .1).hit) {                                           \
                                                                        \
    control.kind = CONTROL_POS_X + axis;                                \
                                                                        \
    Vector3 nearest = NEAREST_POINT_LINE(axis);                         \
                                                                        \
    control.adjustment =                                                \
        (axis == X) * (pos.x - nearest.x) +                             \
        (axis == Y) * (pos.y - nearest.y) +                             \
        (axis == Z) * (pos.z - nearest.z);                              \
                                                                        \
    break;                                                              \
}

#define TOUCH_CONTROL_SIZE(axis) else if(                               \
    GetRayCollisionBox(ray, boundingBox_sized(Vector3Add(pos, (Vector3) \
                {(axis==X)*size.x,(axis==Y)*size.y,(axis==Z)*size.z}    \
                ), 0.2)).hit) {                                         \
    control.kind = CONTROL_SCALE_X + axis;                              \
                                                                        \
    Vector3 nearest = NEAREST_POINT_LINE(axis);                         \
                                                                        \
    control.adjustment =                                                \
        (axis == X) * (size.x - nearest.x) +                            \
        (axis == Y) * (size.y - nearest.y) +                            \
        (axis == Z) * (size.z - nearest.z);                             \
                                                                        \
    break;                                                              \
}

#define TOUCH_CONTROL_ROT(axis, rs) else if(                            \
        rs.hit &&                                                       \
            axis == X ? fabs(rs.normal.z) < 0.01                        \
            : axis == Y ? fabs(rs.normal.x) < 0.01                      \
            : fabs(rs.normal.y) < 0.01                                  \
        ) {                                                             \
    control.kind = CONTROL_ANGLE_X + axis;                              \
                                                                        \
    Vector3 nearest = nearest_point_sphere(ray.position, pos, RING_RADIUS);\
                                                                        \
    control.adjustment =                                                \
        (axis == X) * (pos.x - nearest.x) +                             \
        (axis == Y) * (pos.y - nearest.y) +                             \
        (axis == Z) * (pos.z - nearest.z);                              \
                                                                        \
    break;                                                              \
}

struct Control control(Object *obj, Ray ray) {
    const Vector3 pos = obj->position;
    const Vector3 size = obj->size;

    struct Control control = {0};

    RayCollision rs = GetRayCollisionSphere(ray, pos, RING_RADIUS);
    for(uint8_t axis = 0; axis < 3; axis++) {
        if(false) {}
        TOUCH_CONTROL_POS(axis)
        TOUCH_CONTROL_SIZE(axis)
        TOUCH_CONTROL_ROT(axis, rs)
    }

    return control;
}

void apply_manipulation(struct Control *control, Object *obj, Ray ray) {
    const Vector3 pos = obj->position;

    const uint8_t variant = (control->kind - 1) % 3;
    const uint8_t kind = floor((control->kind - 1) / 3.0 - (FLT_MIN * (variant != 0)));
    // TODO: rotation control
    const Vector3 nearest = /* kind == 2 ? nearest_point_sphere(ray.position, pos, RING_RADIUS) : */ NEAREST_POINT_LINE(variant);
    Vector3 *const target = (Vector3 *const)((Vector3 *const)obj + kind);

    *(float *const)((float *const)target + variant) = control->adjustment + *(float *const)((float *const)&nearest + variant);
}

DynShader action_keybinds(DA *da, int16_t selection) {
    Object obj = DEFAULT_OBJECT;

    // TODO: escape resets seleciton to NO_SELECTION
    if(IsKeyPressed(KEY_SPACE))
        return add_object(da, &obj, selection);
    else if(IsKeyPressed(KEY_X))
        return remove_object(da, selection);


    return (DynShader){0};
}

DynShader add_object(DA *da, Object *obj, int16_t selection) {
    DA_push(da, obj);
    return object_map(da, selection, false);
}

DynShader remove_object(DA *da, int16_t selection) {
    if(selection == NO_SELECTION)
        return (DynShader){0};
    DA_remove(da, selection);
    return object_map(da, NO_SELECTION, false);
}
