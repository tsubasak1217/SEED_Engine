#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Functions/MatrixFunc.h>
#include <SEED/Lib/Functions/MyMath.h>

Vector2 Vector2::operator*(const Matrix3x3& mat) const{
    return Multiply(*this, mat);
}

void Vector2::operator*=(const Matrix3x3& mat){
    *this = Multiply(*this, mat);
}

Vector3 Vector2::ToVec3() const{
    return { x, y, 0 };
}

void Vector2::Normalize(){
    Vector2 normalized = MyMath::Normalize(*this);
    x = normalized.x;
    y = normalized.y;
}

Vector2 Vector2::Normalized() const{
    return MyMath::Normalize(*this);
}

float Vector2::Length() const{
    return MyMath::Length(*this);
}