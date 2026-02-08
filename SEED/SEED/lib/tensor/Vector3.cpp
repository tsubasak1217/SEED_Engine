#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Functions/Math.h>

Vector4 Vector3::ToVec4()
{
    return {x,y,z,1.0f};
}

void Vector3::Normalize(){
    Vector3 normalized = SEED::Methods::Math::Normalize(*this);
    x = normalized.x;
    y = normalized.y;
    z = normalized.z;
}

Vector3 Vector3::Normalized() const{
    return SEED::Methods::Math::Normalize(*this);
}

float Vector3::Length() const{
    return SEED::Methods::Math::Length(*this);
}