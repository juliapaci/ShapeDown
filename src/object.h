#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>

#define TYPE_OR_ZERO(type, val) (Vector3){(type == 0) * val, (type == 1) * val, (type == 2) * val}
#define TYPE_OR_ZERO_VEC3(type, vec3) (Vector3){(type == 0) * vec3.x, (type == 1) * vec3.y, (type == 2) * vec3.z}
#define TYPE_OR_ZERO_COLOUR(type) ((axis==X) ? RED : (axis==Y) ? GREEN : BLUE)
enum Axis {
    X, Y, Z
};

#define RING_RADIUS 2

#define DEBUG_PRINT_OBJECT(obj) printf("position: (%f, %f, %f)\nsize: (%f, %f, %f)\nrotation: (%f, %f, %f)\nradius: %f\nblobbyness: %f\ncolour: (%d, %d, %d)\nmirror: (%b, %b, %b)\nsubtract: %b\n", (obj).position.x, (obj).position.y, (obj).position.z, (obj).size.x, (obj).size.y, (obj).size.z, (obj).rotation.x, (obj).rotation.y, (obj).rotation.z, (obj).radius, (obj).blobbyness, (obj).colour.r, (obj).colour.g, (obj).colour.b, (obj).mirror.x, (obj).mirror.y, (obj).mirror.z, (obj).subtract);

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

// TODO: maybe use linked list instead?

// Dynamic Array
typedef struct {
    uint16_t amount;
    uint16_t capacity;
    Object *array;
} DA;

void DA_init(DA *da, uint16_t size);
void DA_push(DA *da, Object *obj);
void DA_remove(DA *da, uint16_t index);
void DA_free(DA *da);

// Dynamic Shader
typedef struct {
    Shader shader;
    // locations
    union {
        struct {
            int resolution;
            int view_center;
            int view_eye;
            int object_props;
        };

        int z_slice; // for slice see marching cubes
    };
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
    CONTROL_ANGL_Z,
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
void draw_gizmos(Object *obj, ControlKind control);
// returns the users object manipulation
struct Control control(Object *obj, Ray ray);
// apply control to the object
void apply_manipulation(struct Control *control, Object *obj, Ray ray);

// adds an object, returns the reloaded shader
DynShader add_object(DA *da, Object *obj, int16_t selection);
DynShader remove_object(DA *da, int16_t selection);

#endif // __OBJECT_H__
