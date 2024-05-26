#include "helper.h"
#include <raymath.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

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

BoundingBox boundingBox_sized(Vector3 center, float size) {
    return (BoundingBox) {
        .min = Vector3SubtractValue(center, size/2),
        .max = Vector3AddValue(center, size/2)
    };
}

// TODO: dynamic entry with uniforms
const char *object_map_entry(Object *obj) {
    const char *position = TextFormat("point - vec3(%f, %f, %f)", obj->position.x, obj->position.y, obj->position.z);
    const char *size = TextFormat("vec3(%f, %f, %f)", obj->size.x, obj->size.y, obj->size.z);

    const char *block = TextFormat("\tdistance = min(\n%s,\n\t\tdistance);\n",
            TextFormat("\t\tsdf_round_box(\n\t\t\t%s,\n\t\t\t%s,\n\t\t\t%f)", position, size, obj->radius));

    return block;
}

DynShader object_map(DA *da) {
    char *prelude = _read_file("src/shader.glsl");
    char *base = _read_file("src/base.glsl");
    const char *sig = "\nfloat map(vec3);\n";
    const char *map_start = "float map(vec3 point) {\n"
        "\tfloat distance = MAX_RAY_DIST;\n";
    const char *map_end = "\treturn distance;\n"
        "}\0";

    _append(&prelude, sig);
    _append(&prelude, base);
    _append(&prelude, map_start);
    for(size_t i = 0; i < da->amount; i++)
        _append(&prelude, object_map_entry(&da->array[i]));
    _append(&prelude, map_end);

    DynShader shader = {
        .shader       = LoadShaderFromMemory(0, prelude),
        .resolution   = GetShaderLocation(shader.shader, "resolution"),
        .view_eye     = GetShaderLocation(shader.shader, "view_eye"),
        .view_center  = GetShaderLocation(shader.shader, "view_center")
    };

    free(prelude);

    return shader;
}

size_t object_at_pos(Vector2 pos) {
    // object_map();
    return -1;
}
