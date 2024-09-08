#ifndef __HELPER_H__
#define __HELPER_H__

#include <stdlib.h>
#include "raylib.h"
#include "object.h"

#define NO_SELECTION (int16_t)-1
#define NO_OBJECT NULL
#define SELECTED_ID(id) (id != NO_SELECTION)
#define SELECTED_OBJ(obj) (obj != NO_OBJECT)
#define SELECTED(id, obj) (SELECTED_ID(id) && SELECTED_OBJ(obj))

void _append(char **str1, const char *str2);
char *_read_file(const char *file_path);

// bounding box (rect) from a size
BoundingBox boundingBox_sized(Vector3 center, float size);
// Find the point on line p1 to p2 nearest to line p2 to p4
Vector3 NearestPointOnLine(
    Vector3 p1,
    Vector3 p2,
    Vector3 p4,
    Vector3 p3
);
// Find the nearest point on sphere
Vector3 nearest_point_sphere(Vector3 p, Vector3 c, float r);

// gets mirror function for object
const char *mirror_function(Object *obj);
// sets the dynamic selection uniform to selected objects properties
void object_dynamic_assignment(DynShader *shader, Object *obj);
// append static (unselected, constant properties like pos, rot, etc.) object to shader ray march map
const char *object_static_map_entry(Object *obj, int16_t index);
// append dynamnic (selected, uniform properties) object to shader ray march map
const char *object_dynamic_map_entry(Object *obj);
// create the entire (with all objects) object map
DynShader object_map(DA *da, int16_t selection, bool colour_index);

// index (colour) of pixel at `pos`
int16_t object_at_pos(DA *objects, Vector2 pos, Camera *camera);

#endif // __HELPER_H__
