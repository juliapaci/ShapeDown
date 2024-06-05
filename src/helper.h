#ifndef __HELPER_H__
#define __HELPER_H__

#include <stdlib.h>
#include "raylib.h"
#include "object.h"

void _append(char **str1, const char *str2);
char *_read_file(const char *file_path);

// bounding box (rect) from a size
BoundingBox boundingBox_sized(Vector3 center, float size);

// sets the dynamic selection uniform to selected objects properties
void object_dynamic_assignment(float properties[3*15]);
// append static (unselected, constant properties like pos, rot, etc.) object to shader ray march map
const char *object_static_map_entry(Object *obj);
// append dynamnic (selected, uniform properties) object to shader ray march map
const char *object_dynamic_map_entry(Object *obj);
// create the entire (with all objects) object map
// NOTE: selection is 1 based, 0 is used for no selection
DynShader object_map(DA *da, size_t selection);

// inedx (colour) of pixel at `pos`
size_t object_at_pos(Vector2 pos, DynShader *shader);

#endif // __HELPER_H__
