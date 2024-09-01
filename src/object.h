#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>

#define TYPE_OR_ZERO(type, val) (Vector3){(type == 0) * val, (type == 1) * val, (type == 2) * val}

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

extern Object DEFAULT_OBJECT;

// TODO: use linked list instead

// Dynamic Array
typedef struct {
    size_t amount;
    size_t capacity;
    Object *array;
} DA;

void DA_init(DA *da, size_t size);
void DA_push(DA *da, Object *obj);
void DA_remove(DA *da, size_t index);
void DA_free(DA *da);

// Dynamic Shader
typedef struct {
    Shader shader;
    // locations
    int resolution;
    int view_center;
    int view_eye;
    int object_props;
} DynShader;

typedef enum {
    CONTROL_NONE,
    CONTROL_POS_X,
    CONTROL_POS_Y,
    CONTROL_POS_Z,
    CONTROL_SCALE_X,
    CONTROL_SCALE_Y,
    CONTROL_SCALE_Z,
    CONTROL_ANGLE_X,
    CONTROL_ANGLE_Y,
    CONTROL_ANGLE_Z,
    CONTROL_COLOR_R,
    CONTROL_COLOR_G,
    CONTROL_COLOR_B,
    CONTROL_TRANSLATE,
    CONTROL_ROTATE,
    CONTROL_SCALE,
    CONTROL_CORNER_RADIUS,
    CONTROL_ROTATE_CAMERA,
    CONTROL_BLOB_AMOUNT,
} ControlKind;

struct Control {
    ControlKind kind;
    float adjustment;
};

// updates uniform values
void update_shader_uniforms(DynShader *shader, Camera *camera);

// take action on any kebind thats pressed
// `DynShader.shader.id == 0` on no keybind
DynShader action_keybinds(DA *da, int16_t selection);
// draws object gizmos
void draw_gizmos(Object *obj);
// returns the users object manipulation
struct Control control(Object *obj, Ray ray);
// apply control to the object
void apply_manipulation(struct Control *control, Object *obj, Ray ray);

// adds an object, returns the reloaded shader
DynShader add_object(DA *da, Object *obj, int16_t selection);
DynShader remove_object(DA *da, int16_t selection);

#endif // __OBJECT_H__
