#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include <SEED/Lib/Functions/MatrixFunc.h>

Matrix4x4 Matrix3x3::ToMat4x4() const{
    Matrix4x4 mat4 = IdentityMat4();

    // z回転、xyスケール、xy平面移動を4x4行列に反映
    mat4.m[0][0] = m[0][0];
    mat4.m[0][1] = m[0][1];
    mat4.m[1][0] = m[1][0];
    mat4.m[1][1] = m[1][1];
    mat4.m[3][0] = m[2][0];
    mat4.m[3][1] = m[2][1];

    return mat4;
}
