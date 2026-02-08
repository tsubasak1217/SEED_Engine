#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Functions/MatrixMath.h>
#include <SEED/Lib/Functions/Math.h>

Vector2 Vector2::operator*(const Matrix3x3& mat) const{
    return SEED::Methods::Matrix::Multiply(*this, mat);
}

void Vector2::operator*=(const Matrix3x3& mat){
    *this = SEED::Methods::Matrix::Multiply(*this, mat);
}

Vector3 Vector2::ToVec3() const{
    return { x, y, 0 };
}

void Vector2::Normalize(){
    Vector2 normalized = SEED::Methods::Math::Normalize(*this);
    x = normalized.x;
    y = normalized.y;
}

Vector2 Vector2::Normalized() const{
    return SEED::Methods::Math::Normalize(*this);
}

float Vector2::Length() const{
    return SEED::Methods::Math::Length(*this);
}