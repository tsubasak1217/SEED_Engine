#include "AccelerarionField.h"
#include "SEED.h"

bool AccelerationField::CheckCollision(const Vector3& pos){
    if(range.min.x <= pos.x && pos.x <= range.max.x &&
        range.min.y <= pos.y && pos.y <= range.max.y &&
        range.min.z <= pos.z && pos.z <= range.max.z){
        return true;
    }

    return false;
}
