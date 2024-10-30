#include "Quaternion.h"
#include "MyMath.h"

Quaternion::Quaternion(float x, float y, float z, float w){
    // 各成分を設定
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}


// 軸と角度からクォータニオンを作成
Quaternion::Quaternion(const Vector3& axis, float angle){

    // 軸と角度からクォータニオンを作成
    Quaternion q = AngleAxis(angle, axis);

    // 各成分を設定
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}


// コピーコンストラクタ
Quaternion::Quaternion(const Quaternion& q){
    // 各成分をコピー
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}

// クォータニオンの内積
float Quaternion::Dot(const Quaternion& q) const{
    return x * q.x + y * q.y + z * q.z + w * q.w;
}

// クォータニオンの内積
float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2){
    return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

// クォータニオンの正規化
Quaternion Quaternion::Normalize() const{
    // クォータニオンの長さを計算
    float length = std::sqrt(x * x + y * y + z * z + w * w);

    // 各成分を長さで割って正規化
    assert(length != 0);
    return Quaternion(x / length, y / length, z / length, w / length);
}


// クォータニオンの逆数
Quaternion Quaternion::Inverse() const{
    return Quaternion(-x, -y, -z, w);
}


// クォータニオンの球面補間
Quaternion Quaternion::Slerp(const Quaternion& q, float t) const{
    // 内積を計算
    float dot = Dot(q);

    // 内積が負の場合は符号を反転
    Quaternion q2 = q;
    if(dot < 0){
        q2 = q2 * -1;
        dot = -dot;
    }

    // 内積が1に近い場合は線形補間
    if(dot > 0.9995f){
        return Lerp(q2, t);
    }

    // 内積が1に近い場合は線形補間
    float theta = std::acosf(dot);
    float sinTheta = std::sinf(theta);
    float sinTTheta = std::sinf(t * theta);
    float sin1TTheta = std::sinf((1 - t) * theta);

    return ((*this * sin1TTheta) + (q2 * sinTTheta)) / sinTheta;
    
}

// クォータニオンの線形補間
Quaternion Quaternion::Lerp(const Quaternion& q, float t) const{
    return (*this * (1 - t)) + (q * t);
}



// 角度と軸からクォータニオンを作成
Quaternion Quaternion::AngleAxis(float angle, const Vector3& axis)const{

    // 角度をラジアンから半分にしてサインとコサインを計算
    float sin = std::sinf(angle * 0.5f);
    float cos = std::cosf(angle * 0.5f);

    // 単位ベクトルとしての軸を取得
    Vector3 normAxis = MyMath::Normalize(axis);

    // クォータニオンの成分を設定
    return Quaternion(
        normAxis.x * sin,  // x成分
        normAxis.y * sin,  // y成分
        normAxis.z * sin,  // z成分
        cos                // w成分
    );
}


// オイラー角からクォータニオンに変換
Quaternion Quaternion::EulerToQuaternion(const Vector3& eulerRotate) const{
    // オイラー角の各成分
    float cx = std::cosf(eulerRotate.x * 0.5f);
    float sx = std::sinf(eulerRotate.x * 0.5f);
    float cy = std::cosf(eulerRotate.y * 0.5f);
    float sy = std::sinf(eulerRotate.y * 0.5f);
    float cz = std::cosf(eulerRotate.z * 0.5f);
    float sz = std::sinf(eulerRotate.z * 0.5f);

    // クォータニオンの各成分
    float qx = sx * cy * cz + cx * sy * sz;
    float qy = cx * sy * cz - sx * cy * sz;
    float qz = cx * cy * sz - sx * sy * cz;
    float qw = cx * cy * cz + sx * sy * sz;

    return Quaternion(qx,qy, qz, qw);
}


// クォータニオンからオイラー角に変換
Vector3 Quaternion::ToEuler() const{
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
    Vector3 result;
    result.x = std::atan2f(2.0f * (yz + wx), ww - xx - yy + zz);
    result.y = std::asinf(-2.0f * (xz - wy));
    result.z = std::atan2f(2.0f * (xy + wz), ww + xx - yy - zz);

    return result;
}


// クォータニオンから前方ベクトルを取得
Vector3 Quaternion::MakeForward() const{
    // クォータニオンの各成分
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float ww = w * w;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;

    // 前方ベクトルを計算
    Vector3 result;
    result.x = 2.0f * (xz - wy);
    result.y = 2.0f * (yz + wx);
    result.z = ww - xx - yy + zz;

    return MyMath::Normalize(result);
}


// クォータニオンから上方ベクトルを取得
Vector3 Quaternion::MakeUp() const{
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
    Vector3 result;
    result.x = 2.0f * (xy - wz);
    result.y = ww - xx + yy - zz;
    result.z = 2.0f * (yz + wx);

    return MyMath::Normalize(result);
}


// クォータニオンから右方ベクトルを取得
Vector3 Quaternion::MakeRight() const{
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
    Vector3 result;
    result.x = ww + xx - yy - zz;
    result.y = 2.0f * (xy + wz);
    result.z = 2.0f * (xz - wy);

    return MyMath::Normalize(result);
}


// クォータニオンから回転行列を取得
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

//////////////////////////////////////////////////////////
// operator
//////////////////////////////////////////////////////////
Quaternion Quaternion::operator+(const Quaternion& q) const{
    return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}

Quaternion Quaternion::operator*(const Quaternion& q) const{
    return Quaternion(
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w,
        w * q.w - x * q.x - y * q.y - z * q.z
    );
}

Quaternion Quaternion::operator*(float f) const{
    return Quaternion(x * f, y * f, z * f, w * f);
}

Quaternion Quaternion::operator/(float f) const{
    return Quaternion(x / f, y / f, z / f, w / f);
}




