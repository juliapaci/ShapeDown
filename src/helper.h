#ifndef __HELPER_H__
#define __HELPER_H__

#include <stdlib.h>
#include "raylib.h"
#include "object.h"

void _append(char **str1, const char *str2);
char *_read_file(const char *file_path);

BoundingBox boundingBox_sized(Vector3 center, float size);

const char *object_map_entry(Object *obj); // append object to shader ray march map
DynShader object_map(DA *da);

size_t object_at_pos(Vector2 pos, DynShader *shader);

#endif // __HELPER_H__
