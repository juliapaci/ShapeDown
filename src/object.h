#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    Vector3 position;
    Vector3 size;
    Vector3 rotation;

    float radius;
    float blobbyness;

    struct {
        uint8_t r, g, b;
    } colour;

    struct {
        bool x, y, z;
    } mirror;

    bool subtract;
} Object;

// Dynamic Array
typedef struct {
    size_t amount;
    size_t capacity;
    Object *array;
} DA;

void DA_init(DA *da, size_t size);
void DA_push(DA *da, Object *obj);
void DA_free(DA *da);

typedef struct {
    Shader shader;
    int resolution;
    int view_center;
    int view_eye;
    int object_props;
} DynShader;

// updates uniform values
void update_shader_uniforms(DynShader *shader, Camera *camera);

// draws object gizmos e.g. movie, scale, etc.
void draw_gizmos(Object *obj);

// adds an object, returns the reloaded shader
DynShader add_object(DA *da, Object *obj);

#endif // __OBJECT_H__
