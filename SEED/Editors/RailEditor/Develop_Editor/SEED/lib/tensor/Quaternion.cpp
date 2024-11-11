#include "Quaternion.h"

Matrix4x4 Quaternion::MakeMatrix() const{
    // クォータニオンの各成分
    Matrix4x4 result;
    float xx = axis.x * axis.x;
    float yy = axis.y * axis.y;
    float zz = axis.z * axis.z;
    float ww = angle * angle;
    float xy = axis.x * axis.y;
    float xz = axis.x * axis.z;
    float yz = axis.y * axis.z;
    float wx = angle * axis.x;
    float wy = angle * axis.y;
    float wz = angle * axis.z;

    result.m[0][0] = ww + xx - yy - zz;
    result.m[0][1] = 2.0f * (xy + wz);
    result.m[0][2] = 2.0f * (xz - wy);
    result.m[0][3] = 0.0f;

    result.m[1][0] = 2.0f * (xy - wz);
    result.m[1][1] = ww - xx + yy - zz;
    result.m[1][2] = 2.0f * (yz + wx);
    result.m[1][3] = 0.0f;

    result.m[2][0] = 2.0f * (xz + wy);
    result.m[2][1] = 2.0f * (yz - wx);
    result.m[2][2] = ww - xx - yy + zz;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}