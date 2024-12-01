#include "Vector2.h"
#include "Vector3.h"

Vector3 Vector2::ToVec3() const{
    return { x, y, 0 };
}
