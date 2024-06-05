#include "helper.h"
#include <raymath.h>
#include <rlgl.h>
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

void object_dynamic_assignment(DynShader *shader, Object *obj) {
    const float properties[3*5] = {
        obj->position.x,
        obj->position.y,
        obj->position.z,

        obj->size.x,
        obj->size.y,
        obj->size.z,

        obj->rotation.x,
        obj->rotation.y,
        obj->rotation.z,

        // [0..256 - 0..=1]
        obj->colour.r / 255.0f,
        obj->colour.g / 255.0f,
        obj->colour.b / 255.0f,

        // radius, blobbyness, padding
        fmaxf(0.01,
            fminf(obj->radius,
                fminf(obj->size.x,
                    fminf(obj->size.y, obj->size.z)
                )
            )
        ),
        fmaxf(obj->blobbyness, 0.0001),
        0.0f
    };

    SetShaderValueV(
        shader->shader,
        shader->object_props,
        properties,
        SHADER_UNIFORM_VEC3,
        5
    );
}

const char *object_static_map_entry(Object *obj) {
    const char *position = TextFormat("point - vec3(%f, %f, %f)", obj->position.x, obj->position.y, obj->position.z);
    const char *size = TextFormat("vec3(%f, %f, %f)", obj->size.x, obj->size.y, obj->size.z);

    const char *block = TextFormat("\tdistance = min(\n%s,\n\t\tdistance);\n",
            TextFormat("\t\tsdf_round_box(\n\t\t\t%s,\n\t\t\t%s,\n\t\t\t%f)", position, size, obj->radius));

    return block;
}

const char *object_dynamic_map_entry(Object *obj) {
    const char *position = "point - object_props[0]";
    const char *size = "object_props[1]";

    const char *block = TextFormat("\tdistance = min(\n%s,\n\t\tdistance);\n",
            TextFormat("\t\tsdf_round_box(\n\t\t\t%s,\n\t\t\t%s,\n\t\t\t%f)", position, size, obj->radius));

    return block;
}

DynShader object_map(DA *da, DynShader *shader, size_t selection) {
    char *map = NULL;

    char *prelude = _read_file("src/shader.glsl");
    char *base = _read_file("src/base.glsl");
    const char *sig = "\nfloat map(vec3);\n";
    const char *map_start = "float map(vec3 point) {\n"
        "\tfloat distance = MAX_RAY_DIST;\n";
    const char *map_end = "\treturn distance;\n"
        "}\0";

    _append(&map, prelude);
    _append(&map, sig);
    _append(&map, base);
    _append(&map, map_start);
    for(size_t i = 0; i < da->amount; i++) {
        const char *entry = NULL;

        if(i == selection) {
            object_dynamic_assignment(shader, da->array + i);
            entry = object_dynamic_map_entry(da->array + i);
        } else
            entry = object_static_map_entry(da->array + i);

        _append(&map, entry);
    }
    _append(&map, map_end);

    DynShader new_shader = {
        .shader       = LoadShaderFromMemory(0, map),
        .resolution   = GetShaderLocation(new_shader.shader, "resolution"),
        .view_eye     = GetShaderLocation(new_shader.shader, "view_eye"),
        .view_center  = GetShaderLocation(new_shader.shader, "view_center"),
        .object_props = GetShaderLocation(new_shader.shader, "object_props")
    };

    free(map);

    return new_shader;
}

size_t object_at_pos(Vector2 pos, DynShader *shader) {
    RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    BeginTextureMode(target);
        BeginShaderMode(shader->shader);
            rlBegin(RL_QUADS);
            rlTexCoord2f(pos.x-1, pos.y-1);
            rlVertex2f(pos.x-1, pos.y-1);

            rlTexCoord2f(pos.x-1, pos.y+1);
            rlVertex2f(pos.x-1, pos.y+1);

            rlTexCoord2f(pos.x+1, pos.y+1);
            rlVertex2f(pos.x+1, pos.y+1);

            rlTexCoord2f(pos.x+1, pos.y-1);
            rlVertex2f(pos.x+1, pos.y-1);
            rlEnd();
        EndShaderMode();
    EndTextureMode();

    uint8_t *pixels = rlReadTexturePixels(target.texture.id, target.texture.width, target.texture.height, target.texture.format);
    int object_index = pixels[(int)(pos.x + target.texture.width*(target.texture.height - pos.y))*4] - 1;

    free(pixels);
    UnloadRenderTexture(target);

    return object_index;
}
