#include "helper.h"

#include <raymath.h>
#include <rlgl.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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

// Copied directly from ShapeUp-public
Vector3 NearestPointOnLine(
    Vector3 p1,
    Vector3 p2,
    Vector3 p4,
    Vector3 p3
) {
    float mua;

    Vector3 p13,p43,p21;
    float d1343,d4321,d1321,d4343,d2121;
    float numer,denom;

    const float EPS = 0.001;

    p13.x = p1.x - p3.x;
    p13.y = p1.y - p3.y;
    p13.z = p1.z - p3.z;
    p43.x = p4.x - p3.x;
    p43.y = p4.y - p3.y;
    p43.z = p4.z - p3.z;
    if (fabs(p43.x) < EPS && fabs(p43.y) < EPS && fabs(p43.z) < EPS)
        return(Vector3){};
    p21.x = p2.x - p1.x;
    p21.y = p2.y - p1.y;
    p21.z = p2.z - p1.z;
    if (fabs(p21.x) < EPS && fabs(p21.y) < EPS && fabs(p21.z) < EPS)
        return(Vector3){};

    d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
    d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
    d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
    d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
    d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

    denom = d2121 * d4343 - d4321 * d4321;
    if (fabs(denom) < EPS)
        return (Vector3){};
    numer = d1343 * d4321 - d1321 * d4343;

    mua = numer / denom;

    return (Vector3){
        p1.x + mua * p21.x,
        p1.y + mua * p21.y,
        p1.z + mua * p21.z
    };
}

Vector3 nearest_point_sphere(Vector3 p, Vector3 c, float r) {
    Vector3 dir = Vector3Normalize(Vector3Subtract(c, p));
    return Vector3Add(c, Vector3Scale(dir, r));
}

void object_dynamic_assignment(DynShader *shader, Object *obj) {
    const float properties[3*5] = {
        obj->position.x,
        obj->position.y,
        obj->position.z,

        fabs(obj->size.x),
        fabs(obj->size.y),
        fabs(obj->size.z),

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

const char *object_static_map_entry(Object *obj, int16_t index) {
    const char *position = TextFormat("point - vec3(%f, %f, %f)", obj->position.x, obj->position.y, obj->position.z);
    const char *size = TextFormat("vec3(%f, %f, %f)", fabs(obj->size.x), fabs(obj->size.y), fabs(obj->size.z));

    double r, g, b;
    if(index == -1) {
        r = obj->colour.r/255.0;
        g = obj->colour.g/255.0;
        b = obj->colour.b/255.0;
    } else {
        // TODO: could also use another buffer like depth or something
        r = index/255.0;
        g = fmax(index - 255, 0)/255.0;
        b = fmax(index - 255*2, 0)/255.0;
    }

    const char *colour = TextFormat("\n\t\tvec3(%f, %f, %f)", r, g, b);

    const char *block = TextFormat("\tdistance = Min(\n\t\tvec4(%s, %s),\n\t\tdistance);\n",
            TextFormat("sdf_round_box(\n\t\t\t%s,\n\t\t\t%s,\n\t\t\t%f)", position, size, obj->radius),
            colour);

    return block;
}

const char *object_dynamic_map_entry(Object *obj) {
    const char *position = "point - object_props[0]";
    const char *size = "object_props[1]";
    const char *colour = "object_props[3]";

    // TODO: radius should be from object_props
    const char *block = TextFormat("\tdistance = Min(\n\t\tvec4(%s, %s),\n\t\tdistance);\n",
            TextFormat("sdf_round_box(\n\t\t\t%s,\n\t\t\t%s,\n\t\t\t%f)", position, size, obj->radius),
            colour);

    return block;
}

DynShader object_map(DA *da, int16_t selection, bool colour_index) {
    char *map = NULL;

    char *prelude = _read_file("src/shader.glsl");
    char *base = _read_file("src/base.glsl");
    const char *sig = "\nvec4 map(vec3);\n";
    const char *map_start = "vec4 map(vec3 point) {\n" // vec4 (distance, colour)
        "\tvec4 distance = vec4(MAX_RAY_DIST, vec3(0));\n";
    const char *map_end = "\treturn distance;\n"
        "}\0";

    _append(&map, prelude);
    _append(&map, sig);
    _append(&map, base);
    _append(&map, map_start);
    for(uint16_t i = 0; i < da->amount; i++) {
        const char *entry = NULL;

        if(i != selection)
            entry = object_static_map_entry(da->array + i, colour_index * (i + 1) - 1);
        else
            entry = object_dynamic_map_entry(da->array + i);

        _append(&map, entry);
    }
    _append(&map, map_end);

    DynShader shader = {
        .shader       = LoadShaderFromMemory(NULL, map),
        .resolution   = GetShaderLocation(shader.shader, "resolution"),
        .view_eye     = GetShaderLocation(shader.shader, "view_eye"),
        .view_center  = GetShaderLocation(shader.shader, "view_center"),
        .object_props = GetShaderLocation(shader.shader, "object_props")
    };

    return shader;
}

// TODO: check if object with selected id exists or find another qway of prevent picking background colour
int16_t object_at_pos(DA *objects, Vector2 pos, Camera *camera) {
    DynShader shader = object_map(objects, NO_SELECTION, true);
    update_shader_uniforms(&shader, camera);

    RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    BeginTextureMode(target);
        BeginShaderMode(shader.shader);
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

    const size_t index = (int)(pos.x + target.texture.width*(target.texture.height - pos.y))*4;
    const uint8_t r = pixels[index];
    const uint8_t g = pixels[index + 1];
    const uint8_t b = pixels[index + 2];
    const int16_t object_id = r + g + b;

    free(pixels);
    UnloadRenderTexture(target);
    return object_id;
}
