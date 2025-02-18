#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>

void Vector4::operator*=(const Matrix4x4& matrix){

    Vector3 vec3Value = ToVec3();
    vec3Value *= matrix;

    x = vec3Value.x;
    y = vec3Value.y;
    z = vec3Value.z;
    w = 1.0f;
}

Vector3 Vector4::ToVec3()const{
    assert(w != 0.0f);
    return { x / w,y / w ,z / w };
}
