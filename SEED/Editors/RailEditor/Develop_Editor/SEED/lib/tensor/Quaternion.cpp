#include "Quaternion.h"

Matrix4x4 Quaternion::MakeMatrix() const{
    // クォータニオンの各成分
    Matrix4x4 result;
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float ww = w * w;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

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