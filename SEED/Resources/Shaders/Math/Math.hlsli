#ifndef MATH_INCLUDED
#define MATH_INCLUDED

// thetaとphiを使って球面上のベクトルを生成する関数
float3 CreateVector(float theta, float phi) {
    float x = sin(phi) * cos(theta);
    float y = cos(phi);
    float z = sin(phi) * sin(theta);
    return float3(x, y, z);
}

float3 Mul(float3 v, float4x4 mat) {
    float4 converted = float4(v, 1.0f);
    converted = mul(converted, mat);
    return float3(converted.x, converted.y, converted.z);
}

float4x4 AffineMatrix(float3 scale, float3 rotationEuler, float3 translation) {
    // スケール行列
    float4x4 S = float4x4(
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1
    );

    // 回転行列（XYZオイラー）
    float cx = cos(rotationEuler.x);
    float sx = sin(rotationEuler.x);
    float cy = cos(rotationEuler.y);
    float sy = sin(rotationEuler.y);
    float cz = cos(rotationEuler.z);
    float sz = sin(rotationEuler.z);

    float4x4 Rx = float4x4(
        1, 0, 0, 0,
        0, cx, -sx, 0,
        0, sx, cx, 0,
        0, 0, 0, 1
    );

    float4x4 Ry = float4x4(
        cy, 0, sy, 0,
        0, 1, 0, 0,
        -sy, 0, cy, 0,
        0, 0, 0, 1
    );

    float4x4 Rz = float4x4(
        cz, -sz, 0, 0,
        sz, cz, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

    float4x4 R = mul(mul(Rz, Ry), Rx); // 回転の順序：Z→Y→X

    // 平行移動行列
    float4x4 T = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        translation.x, translation.y, translation.z, 1
    );

    // 合成（スケール→回転→平行移動）
    return mul(mul(S, R), T);
}

float4x4 AffineMatrix(float3 scale, float4 quaternion, float3 translation) {
    float4x4 S = float4x4(
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1
    );

    float x = quaternion.x, y = quaternion.y, z = quaternion.z, w = quaternion.w;

    float4x4 R = float4x4(
        1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w), 0,
            2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w), 0,
            2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y), 0,
                            0, 0, 0, 1
    );

    float4x4 T = float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        translation.x, translation.y, translation.z, 1
    );

    return mul(mul(S, R), T);
}

float4x4 RotateMatrix(float3 rotationEuler) {
    float cx = cos(rotationEuler.x);
    float sx = sin(rotationEuler.x);
    float cy = cos(rotationEuler.y);
    float sy = sin(rotationEuler.y);
    float cz = cos(rotationEuler.z);
    float sz = sin(rotationEuler.z);
    return float4x4(
        cy * cz, -cy * sz, sy, 0,
        sx * sy * cz + cx * sz, -sx * sy * sz + cx * cz, -sx * cy, 0,
        -cx * sy * cz + sx * sz, cx * sy * sz + sx * cz, cx * cy, 0,
        0, 0, 0, 1
    );
}

float4x4 RotateMatrix(float4 quaternion) {
    float x = quaternion.x, y = quaternion.y, z = quaternion.z, w = quaternion.w;
    return float4x4(
        1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w), 0,
            2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w), 0,
            2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y), 0,
                            0, 0, 0, 1
    );
}

#endif