#include "object.h"
#include <raylib.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>

void update_shader_locations(DynShader *shader, Camera camera) {
    SetShaderValue(shader->shader, shader->resolution,
        (float[2]){(float)GetScreenWidth(), (float)GetScreenHeight()},
        SHADER_UNIFORM_VEC2
    );
    SetShaderValue(shader->shader, shader->view_eye,
        &camera.position,
        SHADER_UNIFORM_VEC3
    );
    SetShaderValue(shader->shader, shader->view_center,
        &camera.target,
        SHADER_UNIFORM_VEC3
    );
}

char *_read_file(const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if(file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    if(buffer == NULL) {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    fclose(file);
    return buffer;
}

void _append(char **str1, const char *str2) {
    assert(str1);
    assert(str2);
    size_t len = (*str1 ? strlen(*str1) : 0) + strlen(str2);

    char *concatenated = (char *)malloc(len + 1);
    concatenated[0] = 0;
    assert(concatenated);

    if (*str1) {
        strcpy(concatenated, *str1);
        free(*str1);
    }
    strcat(concatenated, str2);

    *str1 = concatenated;
}

DynShader object_map(Object *obj, const char *path) {
    char *prelude = _read_file("src/shader.glsl");
    char *base = _read_file("src/base.glsl");
    char *sig = "\nfloat map(vec3);\n";

    const char *map_start = "float map(vec3 point) {\n"
    "\tfloat distance = MAX_RAY_DIST;\n";
    const char *map_end = "\treturn distance;\n"
        "}\0";

    const char *position = TextFormat("point - vec3(%f, %f, %f)", obj->position.x, obj->position.y, obj->position.z);
    const char *size = TextFormat("vec3(%f, %f, %f)", obj->size.x, obj->size.y, obj->size.z);

    const char *block = TextFormat("\tdistance = min(\n%s,\n\t\tdistance);\n",
            TextFormat("\t\tsdf_round_box(\n\t\t\t%s,\n\t\t\t%s,\n\t\t\t%f)", position, size, obj->roundedness));

    _append(&prelude, sig);
    _append(&prelude, base);
    _append(&prelude, map_start);
    _append(&prelude, block);
    _append(&prelude, map_end);

    DynShader shader;
    shader.shader = LoadShaderFromMemory(0, prelude);
    shader.resolution = GetShaderLocation(shader.shader, "resolution");
    shader.view_eye = GetShaderLocation(shader.shader, "view_eye");
    shader.view_center = GetShaderLocation(shader.shader, "view_center");

    return shader;
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
