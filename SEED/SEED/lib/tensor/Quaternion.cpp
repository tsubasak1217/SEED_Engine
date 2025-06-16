#include <SEED/Lib/Tensor/Quaternion.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <cmath> 
#include <numbers>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
using namespace DirectX;

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


Quaternion Quaternion::Normalize(const Quaternion& q){
    // クォータニオンの長さを計算
    float length = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

    // 各成分を長さで割って正規化
    assert(length != 0);
    return Quaternion(q.x / length, q.y / length, q.z / length, q.w / length);
}


// クォータニオンの逆
Quaternion Quaternion::Inverse() const{
    // クォータニオンのノルムの2乗を計算
    float normSquared = w * w + x * x + y * y + z * z;

    // ノルムの2乗が0に近い場合、逆クォータニオンは定義できない
    if(normSquared < 0.000001f) {
        assert(false);
        return Quaternion();
    }

    // 共役を計算
    Quaternion conjugate = Conjugate();

    // 逆クォータニオンは共役をノルムの2乗で割ったもの
    return conjugate / normSquared;
}

Quaternion Quaternion::Inverse(const Quaternion& q){
    return q.Inverse();
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

// クォータニオンの球面補間
Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t){
    // 内積を計算
    float dot = Dot(q1, q2);

    // dotを-1.0～1.0の範囲にクランプ
    if(dot < -1.0f) dot = -1.0f;
    if(dot > 1.0f) dot = 1.0f;

    Quaternion q2Adjusted = q2;
    if(dot < 0.0f) {
        // 逆方向の場合は符号を反転
        q2Adjusted = { -q2.x, -q2.y, -q2.z, -q2.w };
        dot = -dot;
    }

    // ほぼ同じ方向の場合は線形補間
    const float EPSILON = 1e-6f;
    if(dot > 1.0f - EPSILON) {
        Quaternion result;
        result.w = q1.w + t * (q2Adjusted.w - q1.w);
        result.x = q1.x + t * (q2Adjusted.x - q1.x);
        result.y = q1.y + t * (q2Adjusted.y - q1.y);
        result.z = q1.z + t * (q2Adjusted.z - q1.z);
        return result.Normalize();
    }

    // 角度を計算
    float theta = std::acos(dot);
    float sinTheta = std::sin(theta);

    // ウェイトを計算
    float weight1 = std::sin((1.0f - t) * theta) / sinTheta;
    float weight2 = std::sin(t * theta) / sinTheta;

    // 補間結果を計算
    Quaternion result;
    result.w = weight1 * q1.w + weight2 * q2Adjusted.w;
    result.x = weight1 * q1.x + weight2 * q2Adjusted.x;
    result.y = weight1 * q1.y + weight2 * q2Adjusted.y;
    result.z = weight1 * q1.z + weight2 * q2Adjusted.z;

    return result;
}

// クォータニオンの球面補間(オイラー角から)
Quaternion Quaternion::Slerp(const Vector3& r1, const Vector3& r2, float t){
    // クォータニオンに変換
    Quaternion q1 = Quaternion::ToQuaternion(r1);
    Quaternion q2 = Quaternion::ToQuaternion(r2);

    // 球面補間
    return q1.Slerp(q2, t);
}

// クォータニオンの線形補間
Quaternion Quaternion::Lerp(const Quaternion& q, float t) const{
    return (*this * (1 - t)) + (q * t);
}


// Quaternionで回転させたベクトルを計算
Vector3 Quaternion::RotatedVector(const Vector3& vec, const Quaternion& q){
    Quaternion r(vec.x, vec.y, vec.z,0.0f);
    Quaternion result = q * r * q.Conjugate();
    return Vector3(result.x, result.y, result.z);
}



// 角度と軸からクォータニオンを作成
Quaternion Quaternion::AngleAxis(float angle, const Vector3& axis){

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
Quaternion Quaternion::ToQuaternion(const Vector3& eulerRotate){
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

    return Quaternion(qx, qy, qz, qw);
}


// クォータニオンからオイラー角に変換
Vector3 Quaternion::ToEuler(const Quaternion& q){
    // クォータニオンの各成分
    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float ww = q.w * q.w;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    // オイラー角を計算
    Vector3 result;
    result.x = std::atan2f(2.0f * (yz + wx), ww - xx - yy + zz);
    result.y = std::asinf(-2.0f * (xz - wy));
    result.z = std::atan2f(2.0f * (xy + wz), ww + xx - yy - zz);

    return result;
}

Quaternion Quaternion::MatrixToQuaternion(const Matrix4x4& mat){

    Quaternion q;
    float r22 = mat.m[2][2];
    if(r22 <= 0.f)  // x^2 + y^2 >= z^2 + w^2
    {
        float dif10 = mat.m[1][1] - mat.m[0][0];
        float omr22 = 1.f - r22;
        if(dif10 <= 0.f)  // x^2 >= y^2
        {
            float fourXSqr = omr22 - dif10;
            float inv4x = 0.5f / sqrtf(fourXSqr);
            q.x = fourXSqr * inv4x;
            q.y = (mat.m[0][1] + mat.m[1][0]) * inv4x;
            q.z = (mat.m[0][2] + mat.m[2][0]) * inv4x;
            q.w = (mat.m[1][2] - mat.m[2][1]) * inv4x;
        } else  // y^2 >= x^2
        {
            float fourYSqr = omr22 + dif10;
            float inv4y = 0.5f / sqrtf(fourYSqr);
            q.x = (mat.m[0][1] + mat.m[1][0]) * inv4y;
            q.y = fourYSqr * inv4y;
            q.z = (mat.m[1][2] + mat.m[2][1]) * inv4y;
            q.w = (mat.m[2][0] - mat.m[0][2]) * inv4y;
        }
    } else  // z^2 + w^2 >= x^2 + y^2
    {
        float sum10 = mat.m[1][1] + mat.m[0][0];
        float opr22 = 1.f + r22;
        if(sum10 <= 0.f)  // z^2 >= w^2
        {
            float fourZSqr = opr22 - sum10;
            float inv4z = 0.5f / sqrtf(fourZSqr);
            q.x = (mat.m[0][2] + mat.m[2][0]) * inv4z;
            q.y = (mat.m[1][2] + mat.m[2][1]) * inv4z;
            q.z = fourZSqr * inv4z;
            q.w = (mat.m[0][1] - mat.m[1][0]) * inv4z;
        } else  // w^2 >= z^2
        {
            float fourWSqr = opr22 + sum10;
            float inv4w = 0.5f / sqrtf(fourWSqr);
            q.x = (mat.m[1][2] - mat.m[2][1]) * inv4w;
            q.y = (mat.m[2][0] - mat.m[0][2]) * inv4w;
            q.z = (mat.m[0][1] - mat.m[1][0]) * inv4w;
            q.w = fourWSqr * inv4w;
        }
    }
    return q;
}



// クォータニオンからオイラー角に変換
Vector3 Quaternion::ToEuler() const{
    // クォータニオンの各成分
    float xx = this->x * this->x;
    float yy = this->y * this->y;
    float zz = this->z * this->z;
    float ww = this->w * this->w;
    float xy = this->x * this->y;
    float xz = this->x * this->z;
    float yz = this->y * this->z;
    float wx = this->w * this->x;
    float wy = this->w * this->y;
    float wz = this->w * this->z;

    // オイラー角を計算
    Vector3 result;
    result.x = std::atan2f(2.0f * (yz + wx), ww - xx - yy + zz);
    result.y = std::asinf(-2.0f * (xz - wy));
    result.z = std::atan2f(2.0f * (xy + wz), ww + xx - yy - zz);

    return result;
}


// クォータニオンから前方ベクトルを取得
Vector3 Quaternion::MakeForward() const{

    Vector3 up = MakeUp();
    Vector3 right = MakeRight();


    return MyMath::Cross(-up, right);
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

// FromベクトルからToベクトルへの回転行列を計算する関数
Matrix4x4 Quaternion::DirectionToDirection(const Vector3& from, const Vector3& to){
    return LookAt(from, to).MakeMatrix();
}

//////////////////////////////////////////////////////////
// LookAt関数
//////////////////////////////////////////////////////////
Quaternion Quaternion::LookAt(const Vector3& from, const Vector3& to){
    // FromベクトルとToベクトルの正規化
    Vector3 fromN = MyMath::Normalize(from);
    Vector3 toN = MyMath::Normalize(to);

    // FromベクトルとToベクトルの内積
    float dot = MyMath::Dot(fromN, toN);

    // FromベクトルとToベクトルが逆方向の場合
    if(dot < -0.999999f) {
        // 任意の軸を選択して180度回転
        Vector3 axis = MyMath::Cross(Vector3(0.0f, 0.0f, 1.0f), fromN);
        if(MyMath::Length(axis) < 0.000001f) {
            axis = MyMath::Cross(Vector3(1.0f, 0.0f, 0.0f), fromN);
        }
        axis = MyMath::Normalize(axis);
        return Quaternion(axis, (float)std::numbers::pi);
    }

    // FromベクトルとToベクトルが同じ方向の場合
    if(dot > 0.999999f) {
        return Quaternion(); // 単位行列を返す
    }

    // 通常のケース（dotが-1 < dot < 1 の場合）
    Vector3 axis = MyMath::Normalize(MyMath::Cross(fromN, toN));  // 外積で回転軸を計算
    float angle = std::acos(dot);  // dotが[-1, 1]の範囲なので安全
    return Quaternion::AngleAxis(angle, axis);
}


// 単位クォータニオン
Quaternion Quaternion::Identity(){
    return Quaternion();
}

// 共役クォータニオン
Quaternion Quaternion::Conjugate() const{
    return Quaternion(-x, -y, -z, w);

}

// 共役クォータニオン
Quaternion Quaternion::Conjugate(const Quaternion& q){
    return Quaternion(-q.x, -q.y, -q.z, q.w);
}

// ノルム
float Quaternion::Norm() const{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

// ノルム
float Quaternion::Norm(const Quaternion& q){
    return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

//////////////////////////////////////////////////////////
// operator
//////////////////////////////////////////////////////////
Quaternion Quaternion::operator+(const Quaternion& q) const{
    return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}

Quaternion Quaternion::operator-(const Quaternion& q) const{
    return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
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

void Quaternion::operator*=(const Quaternion& q){
    // クォータニオンの積を計算
    float newX = w * q.x + x * q.w + y * q.z - z * q.y;
    float newY = w * q.y - x * q.z + y * q.w + z * q.x;
    float newZ = w * q.z + x * q.y - y * q.x + z * q.w;
    float newW = w * q.w - x * q.x - y * q.y - z * q.z;
    // 各成分を更新
    x = newX;
    y = newY;
    z = newZ;
    w = newW;
}

Vector3 Quaternion::operator*(const Vector3& v) const{
    // 1) v を「スカラー部 0、ベクトル部 v」のクォータニオンに変換
    Quaternion qv(v.x, v.y, v.z, 0.0f);

    // 2) qv' = q * qv
    Quaternion qvPrime;
    qvPrime.w = w * qv.w - x * qv.x - y * qv.y - z * qv.z;
    qvPrime.x = w * qv.x + x * qv.w + y * qv.z - z * qv.y;
    qvPrime.y = w * qv.y - x * qv.z + y * qv.w + z * qv.x;
    qvPrime.z = w * qv.z + x * qv.y - y * qv.x + z * qv.w;

    // 3) conj(q) = ( -x, -y, -z, w )  (正規化されている前提)
    Quaternion conjQ(-x, -y, -z, w);

    // 4) qv' * conj(q) を計算すると (スカラー部は捨てる)
    Quaternion result;
    result.w = qvPrime.w * conjQ.w - qvPrime.x * conjQ.x
        - qvPrime.y * conjQ.y - qvPrime.z * conjQ.z;
    result.x = qvPrime.w * conjQ.x + qvPrime.x * conjQ.w
        + qvPrime.y * conjQ.z - qvPrime.z * conjQ.y;
    result.y = qvPrime.w * conjQ.y - qvPrime.x * conjQ.z
        + qvPrime.y * conjQ.w + qvPrime.z * conjQ.x;
    result.z = qvPrime.w * conjQ.z + qvPrime.x * conjQ.y
        - qvPrime.y * conjQ.x + qvPrime.z * conjQ.w;

    // 最後にベクトル部 (x, y, z) を取り出す
    return Vector3(result.x, result.y, result.z);
}

