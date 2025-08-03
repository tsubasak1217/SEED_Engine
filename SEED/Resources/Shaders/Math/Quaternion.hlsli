#ifndef QUATERNION_INCLUDED
#define QUATERNION_INCLUDED
#include "Numbers.hlsli"

// クォータニオンクラス(左手座標系)
struct Quaternion {

    float x;
    float y;
    float z;
    float w;
    
    // Methods
    float Dot(inout Quaternion q);
    Quaternion Normalize();
    static Quaternion Normalize(inout Quaternion q);
    Quaternion Inverse();
    static Quaternion Inverse(inout Quaternion q);
    Quaternion Slerp(inout Quaternion q, float t);
    static Quaternion Slerp(inout Quaternion q1, inout Quaternion q2, float t);
    static Quaternion Slerp(float3 r1, float3 r2, float t);
    Quaternion Lerp(inout Quaternion q, float t);
    static float3 RotatedVector(float3 vec, inout Quaternion q);

    // Convert
    static Quaternion AngleAxis(float angle, float3 axis);
    static Quaternion MatrixToQuaternion(inout float4x4 mat);
    static Quaternion ToQuaternion(float3 eulerRotate);
    static float3 ToEuler(inout Quaternion q);
    float3 ToEuler();
    float4 ToFloat4();
    
    // Make
    float3 MakeForward();
    float3 MakeUp();
    float3 MakeRight();
    float4x4 MakeMatrix();
    static float4x4 DirectionToDirection(float3 from, float3 to);
    static Quaternion LookAt(float3 from, float3 to);
    // 単位クォータニオン
    static Quaternion Identity();
    // 共役クォータニオン
    Quaternion Conjugate();
    // ノルム
    float Norm();
    static float Norm(inout Quaternion q);

    // 演算
    Quaternion Add(inout Quaternion q);
    Quaternion Multiply(inout Quaternion q);
    Quaternion Multiply(float value);
    Quaternion Devide(float value);
    
    // 疑似コンストラクタ
    static Quaternion MakeQuaternion(float x_, float y_, float z_, float w_) {
        Quaternion q;
        q.x = x_;
        q.y = y_;
        q.z = z_;
        q.w = w_;
        return q;
    }
    
    static Quaternion MakeQuaternion(float3 axis, float angle) {
        Quaternion q = AngleAxis(angle, axis);
        return q;
    }
    
};


// クォータニオンの内積
float Quaternion::Dot(inout Quaternion q) {
    return x * q.x + y * q.y + z * q.z + w * q.w;
}


// クォータニオンの正規化
Quaternion Quaternion::Normalize() {
    // クォータニオンの長さを計算
    float length = sqrt(x * x + y * y + z * z + w * w);

    // 各成分を長さで割って正規化
    return MakeQuaternion(x / length, y / length, z / length, w / length);
}


Quaternion Quaternion::Normalize(inout Quaternion q) {
    // クォータニオンの長さを計算
    float length = sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

    // 各成分を長さで割って正規化
    return MakeQuaternion(q.x / length, q.y / length, q.z / length, q.w / length);
}


// クォータニオンの逆
Quaternion Quaternion::Inverse() {
    // クォータニオンのノルムの2乗を計算
    float normSquared = w * w + x * x + y * y + z * z;

    // ノルムの2乗が0に近い場合、逆クォータニオンは定義できない
    if (normSquared < 0.000001f) {
        Quaternion q;
        return q;
    }

    // 共役を計算
    Quaternion conjugate = Conjugate();

    // 逆クォータニオンは共役をノルムの2乗で割ったもの
    return conjugate.Devide(normSquared);
}

Quaternion Quaternion::Inverse(inout Quaternion q) {
    return q.Inverse();
}


// クォータニオンの球面補間
Quaternion Quaternion::Slerp(inout Quaternion q, float t) {
    // 内積を計算
    float dot = Dot(q);

    // 内積が負の場合は符号を反転
    Quaternion q2 = q;
    if (dot < 0) {
        q2 = q2.Multiply(-1);
        dot = -dot;
    }

    // 内積が1に近い場合は線形補間
    if (dot > 0.9995f) {
        return Lerp(q2, t);
    }

    // 内積が1に近い場合は線形補間
    float theta = acos(dot);
    float sinTheta = sin(theta);
    float sinTTheta = sin(t * theta);
    float sin1TTheta = sin((1 - t) * theta);

    Quaternion qSin1TTheta = Multiply(sin1TTheta);
    Quaternion qSinTTheta = Multiply(sinTTheta);
    Quaternion qAdd = qSin1TTheta.Add(q2);
    return qAdd.Devide(sinTheta);

}

// クォータニオンの球面補間
Quaternion Quaternion::Slerp(inout Quaternion q1, inout Quaternion q2, float t) {
    // 内積を計算
    float dot = q1.Dot(q2);

    // dotを-1.0～1.0の範囲にクランプ
    if (dot < -1.0f)
        dot = -1.0f;
    if (dot > 1.0f)
        dot = 1.0f;

    Quaternion q2Adjusted = q2;
    if (dot < 0.0f) {
        // 逆方向の場合は符号を反転
        q2Adjusted = MakeQuaternion(-q2.x, -q2.y, -q2.z, -q2.w);
        dot = -dot;
    }

    // ほぼ同じ方向の場合は線形補間
    const float EPSILON = 1e-6f;
    if (dot > 1.0f - EPSILON) {
        Quaternion result;
        result.w = q1.w + t * (q2Adjusted.w - q1.w);
        result.x = q1.x + t * (q2Adjusted.x - q1.x);
        result.y = q1.y + t * (q2Adjusted.y - q1.y);
        result.z = q1.z + t * (q2Adjusted.z - q1.z);
        return result.Normalize();
    }

    // 角度を計算
    float theta = acos(dot);
    float sinTheta = sin(theta);

    // ウェイトを計算
    float weight1 = sin((1.0f - t) * theta) / sinTheta;
    float weight2 = sin(t * theta) / sinTheta;

    // 補間結果を計算
    Quaternion result;
    result.w = weight1 * q1.w + weight2 * q2Adjusted.w;
    result.x = weight1 * q1.x + weight2 * q2Adjusted.x;
    result.y = weight1 * q1.y + weight2 * q2Adjusted.y;
    result.z = weight1 * q1.z + weight2 * q2Adjusted.z;

    return result;
}

// クォータニオンの球面補間(オイラー角から)
Quaternion Quaternion::Slerp(float3 r1, float3 r2, float t) {
    // クォータニオンに変換
    Quaternion q1 = ToQuaternion(r1);
    Quaternion q2 = ToQuaternion(r2);

    // 球面補間
    return q1.Slerp(q2, t);
}

// クォータニオンの線形補間
Quaternion Quaternion::Lerp(inout Quaternion q, float t){
    Quaternion q2 = Multiply(1 - t);
    Quaternion q3 = q.Multiply(t);
    return q2.Add(q3);
}


// Quaternionで回転させたベクトルを計算
float3 Quaternion::RotatedVector(float3 vec, inout Quaternion q) {
    Quaternion r = MakeQuaternion(vec.x, vec.y, vec.z, 0.0f);
    Quaternion result = q.Multiply(r).Multiply(q.Conjugate());
    return float3(result.x, result.y, result.z);
}



// 角度と軸からクォータニオンを作成
Quaternion Quaternion::AngleAxis(float angle, float3 axis) {

    // 角度をラジアンから半分にしてサインとコサインを計算
    float s = sin(angle * 0.5f);
    float c = cos(angle * 0.5f);

    // 単位ベクトルとしての軸を取得
    float3 normAxis = normalize(axis);

    // クォータニオンの成分を設定
    return MakeQuaternion(
        normAxis.x * s, // x成分
        normAxis.y * s, // y成分
        normAxis.z * s, // z成分
        c // w成分
    );
}


// オイラー角からクォータニオンに変換
Quaternion Quaternion::ToQuaternion(float3 eulerRotate) {
    // オイラー角の各成分
    float cx = cos(eulerRotate.x * 0.5f);
    float sx = sin(eulerRotate.x * 0.5f);
    float cy = cos(eulerRotate.y * 0.5f);
    float sy = sin(eulerRotate.y * 0.5f);
    float cz = cos(eulerRotate.z * 0.5f);
    float sz = sin(eulerRotate.z * 0.5f);

    // クォータニオンの各成分
    float qx = sx * cy * cz + cx * sy * sz;
    float qy = cx * sy * cz - sx * cy * sz;
    float qz = cx * cy * sz - sx * sy * cz;
    float qw = cx * cy * cz + sx * sy * sz;

    return MakeQuaternion(qx, qy, qz, qw);
}


// クォータニオンからオイラー角に変換
float3 Quaternion::ToEuler(inout Quaternion q) {
    float3 euler;

    // Y軸（yaw）
    float siny_cosp = 2.0f * (q.w * q.y + q.z * q.x);
    float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.x * q.x);
    euler.y = atan2(siny_cosp, cosy_cosp);

    // X軸（pitch）
    float sinp = 2.0f * (q.w * q.x - q.y * q.z);
    if (abs(sinp) >= 1)
        euler.x = (PI / 2.0) * sign(sinp); // gimbal lock
    else
        euler.x = asin(sinp);

    // Z軸（roll）
    float sinr_cosp = 2.0f * (q.w * q.z + q.x * q.y);
    float cosr_cosp = 1.0f - 2.0f * (q.z * q.z + q.x * q.x);
    euler.z = atan2(sinr_cosp, cosr_cosp);

    return euler;
}

// クォータニオンをfloat4に変換
float4 Quaternion::ToFloat4() {
    return float4(x, y, z, w);
}

Quaternion Quaternion::MatrixToQuaternion(inout float4x4 mat) {

    Quaternion q;
    float r22 = mat[2][2];
    if (r22 <= 0.f)  // x^2 + y^2 >= z^2 + w^2
    {
        float dif10 = mat[1][1] - mat[0][0];
        float omr22 = 1.f - r22;
        if (dif10 <= 0.f)  // x^2 >= y^2
        {
            float fourXSqr = omr22 - dif10;
            float inv4x = 0.5f / sqrt(fourXSqr);
            q.x = fourXSqr * inv4x;
            q.y = (mat[0][1] + mat[1][0]) * inv4x;
            q.z = (mat[0][2] + mat[2][0]) * inv4x;
            q.w = (mat[1][2] - mat[2][1]) * inv4x;
        } else // y^2 >= x^2
        {
            float fourYSqr = omr22 + dif10;
            float inv4y = 0.5f / sqrt(fourYSqr);
            q.x = (mat[0][1] + mat[1][0]) * inv4y;
            q.y = fourYSqr * inv4y;
            q.z = (mat[1][2] + mat[2][1]) * inv4y;
            q.w = (mat[2][0] - mat[0][2]) * inv4y;
        }
    } else // z^2 + w^2 >= x^2 + y^2
    {
        float sum10 = mat[1][1] + mat[0][0];
        float opr22 = 1.f + r22;
        if (sum10 <= 0.f)  // z^2 >= w^2
        {
            float fourZSqr = opr22 - sum10;
            float inv4z = 0.5f / sqrt(fourZSqr);
            q.x = (mat[0][2] + mat[2][0]) * inv4z;
            q.y = (mat[1][2] + mat[2][1]) * inv4z;
            q.z = fourZSqr * inv4z;
            q.w = (mat[0][1] - mat[1][0]) * inv4z;
        } else // w^2 >= z^2
        {
            float fourWSqr = opr22 + sum10;
            float inv4w = 0.5f / sqrt(fourWSqr);
            q.x = (mat[1][2] - mat[2][1]) * inv4w;
            q.y = (mat[2][0] - mat[0][2]) * inv4w;
            q.z = (mat[0][1] - mat[1][0]) * inv4w;
            q.w = fourWSqr * inv4w;
        }
    }
    return q;
}



// クォータニオンからオイラー角に変換
float3 Quaternion::ToEuler() {
    // クォータニオンの各成分
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

    // オイラー角を計算
    float3 result;
    result.x = atan2(2.0f * (yz + wx), ww - xx - yy + zz);
    result.y = asin(-2.0f * (xz - wy));
    result.z = atan2(2.0f * (xy + wz), ww + xx - yy - zz);

    return result;
}


// クォータニオンから前方ベクトルを取得
float3 Quaternion::MakeForward() {

    float3 up = MakeUp();
    float3 right = MakeRight();
    return cross(-up, right);
}


// クォータニオンから上方ベクトルを取得
float3 Quaternion::MakeUp() {
    // クォータニオンの各成分
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float ww = w * w;
    float xy = x * y;
    float yz = y * z;
    float wx = w * x;
    float wz = w * z;

    // 上方ベクトルを計算
    float3 result;
    result.x = 2.0f * (xy - wz);
    result.y = ww - xx + yy - zz;
    result.z = 2.0f * (yz + wx);

    return normalize(result);
}


// クォータニオンから右方ベクトルを取得
float3 Quaternion::MakeRight() {
    // クォータニオンの各成分
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float ww = w * w;
    float xy = x * y;
    float xz = x * z;
    float wy = w * y;
    float wz = w * z;

    // 右方ベクトルを計算
    float3 result;
    result.x = ww + xx - yy - zz;
    result.y = 2.0f * (xy + wz);
    result.z = 2.0f * (xz - wy);

    return normalize(result);
}


// クォータニオンから回転行列を取得
float4x4 Quaternion::MakeMatrix() {
    // クォータニオンの各成分
    float4x4 result;
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

    result[0][0] = ww + xx - yy - zz;
    result[0][1] = 2.0f * (xy + wz);
    result[0][2] = 2.0f * (xz - wy);
    result[0][3] = 0.0f;

    result[1][0] = 2.0f * (xy - wz);
    result[1][1] = ww - xx + yy - zz;
    result[1][2] = 2.0f * (yz + wx);
    result[1][3] = 0.0f;

    result[2][0] = 2.0f * (xz + wy);
    result[2][1] = 2.0f * (yz - wx);
    result[2][2] = ww - xx - yy + zz;
    result[2][3] = 0.0f;

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;

    return result;
}

// FromベクトルからToベクトルへの回転行列を計算する関数
float4x4 Quaternion::DirectionToDirection(float3 from, float3 to) {
    return LookAt(from, to).MakeMatrix();
}

//////////////////////////////////////////////////////////
// LookAt関数
//////////////////////////////////////////////////////////
Quaternion Quaternion::LookAt(float3 from, float3 to) {
    // FromベクトルとToベクトルの正規化
    float3 fromN = normalize(from);
    float3 toN = normalize(to);

    // FromベクトルとToベクトルの内積
    float d = dot(fromN, toN);

    // FromベクトルとToベクトルが逆方向の場合
    if (d < -0.999999f) {
        // 任意の軸を選択して180度回転
        float3 axis = cross(float3(0.0f, 0.0f, 1.0f), fromN);
        if (length(axis) < 0.000001f) {
            axis = cross(float3(1.0f, 0.0f, 0.0f), fromN);
        }
        axis = normalize(axis);
        return MakeQuaternion(axis, PI);
    }

    // FromベクトルとToベクトルが同じ方向の場合
    if (d > 0.999999f) {
        Quaternion q;
        return q; // 単位行列を返す
    }

    // 通常のケース（dotが-1 < dot < 1 の場合）
    float3 axis = normalize(cross(fromN, toN)); // 外積で回転軸を計算
    float angle = acos(d); // dotが[-1, 1]の範囲なので安全
    return AngleAxis(angle, axis);
}


// 単位クォータニオン
Quaternion Quaternion::Identity() {
    Quaternion q;
    return q;
}

// 共役クォータニオン
Quaternion Quaternion::Conjugate() {
    return MakeQuaternion(-x, -y, -z, w);
}


// ノルム
float Quaternion::Norm() {
    return sqrt(x * x + y * y + z * z + w * w);
}

// ノルム
float Quaternion::Norm(inout Quaternion q) {
    return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}


// 演算
Quaternion Quaternion::Add(inout Quaternion q) {
    return MakeQuaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}

Quaternion Quaternion::Devide(float value) {
    return MakeQuaternion(x / value, y / value, z / value, w / value);
}

Quaternion Quaternion::Multiply(inout Quaternion q) {
    return MakeQuaternion(
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w,
        w * q.w - x * q.x - y * q.y - z * q.z
    );
}

Quaternion Quaternion::Multiply(float value) {
    return MakeQuaternion(x * value, y * value, z * value, w * value);
}


#endif