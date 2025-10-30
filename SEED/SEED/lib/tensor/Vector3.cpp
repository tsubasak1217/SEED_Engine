#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Functions/MyMath.h>

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

Vector3 Vector3::Normalized() const{
    return MyMath::Normalize(*this);
}

float Vector3::Length() const{
    return MyMath::Length(*this);
}