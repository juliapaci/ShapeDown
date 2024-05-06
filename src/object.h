#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    Shader shader;
    int resolution;
    int view_center;
    int view_eye;
} DynShader;

typedef struct {
    Vector3 position;
    Vector3 size;
    Vector3 angle;

    float roundedness;
    float blobbyness;

    struct {
        uint8_t r, g, b;
    } colour;

    struct {
        bool x, y, z;
    } mirror;

    bool subtract;
} Object;

// updates DynShader with proper uniform locations
void update_shader_locations(DynShader *shader, Camera camera);

void _append(char **str1, const char *str2);
char *_read_file(const char *file_path);
DynShader object_map(Object *obj, const char *path); // append object to shader ray march map

// Dynamic Array
typedef struct {
    size_t amount;
    size_t capacity;
    Object *array;
} DA;

void DA_init(DA *da, size_t size);
void DA_push(DA *da, Object *obj);
void DA_free(DA *da);

#endif // __OBJECT_H__
