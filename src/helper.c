#include "helper.h"
#include <raymath.h>

BoundingBox boundingBox_sized(Vector3 center, float size) {
    return (BoundingBox) {
        .min = Vector3SubtractValue(center, size/2),
        .max = Vector3AddValue(center, size/2)
    };
}
