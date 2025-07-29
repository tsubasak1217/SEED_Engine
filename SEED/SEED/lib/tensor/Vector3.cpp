#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>

Vector4 Vector3::ToVec4()
{
    return {x,y,z,1.0f};
}

void Vector3::Normalize(){
    Vector3 normalized = MyMath::Normalize(*this);
    x = normalized.x;
    y = normalized.y;
    z = normalized.z;
}