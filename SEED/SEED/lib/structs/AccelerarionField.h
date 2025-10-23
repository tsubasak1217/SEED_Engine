#pragma once
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Structs/Range3D.h>

/// <summary>
/// 加速場
/// </summary>
struct AccelerationField{

    AccelerationField() = default;
    AccelerationField(const Vector3& acceleration, const Range3D& range)
        : acceleration(acceleration), range(range){}

    Vector3 acceleration;
    Range3D range;

    bool CheckCollision(const Vector3& pos);
    void Draw()const;
};