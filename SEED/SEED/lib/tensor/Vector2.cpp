#include "Vector2.h"
#include "Vector3.h"
#include "MatrixFunc.h"

Vector2 Vector2::operator*(const Matrix3x3& mat) const{
    return Multiply(*this, mat);
}

void Vector2::operator*=(const Matrix3x3& mat){
    *this = Multiply(*this, mat);
}

Vector3 Vector2::ToVec3() const{
    return { x, y, 0 };
}
