#include"localFunc.h"

Matrix4x4 LookAtMatrix(const Vector3& position, const Vector3& target, const Vector3& up){
    // 前方向ベクトルを計算
    Vector3 forward = MyMath::Normalize(target - position);

    // 右方向ベクトルを計算
    Vector3 right = MyMath::Normalize(MyMath::Cross(up, forward));

    // 修正された上方向ベクトルを計算
    Vector3 correctedUp = MyMath::Cross(forward, right);

    // 回転行列を構築
    Matrix4x4 rotationMatrix = {
        right.x,       correctedUp.x,       forward.x,       0.0f,
        right.y,       correctedUp.y,       forward.y,       0.0f,
        right.z,       correctedUp.z,       forward.z,       0.0f,
        0.0f,          0.0f,                0.0f,            1.0f
    };

    return rotationMatrix;
}

Vector3 MatrixToEulerAngles(const Matrix4x4& mat){
    float x, y, z;
    // Y軸の角度を計算
    y = std::asin(-mat.m[2][0]);

    if (std::abs(mat.m[2][0]) < 1.0){
        // 特異点でない場合
        x = std::atan2(mat.m[2][1], mat.m[2][2]);
        z = std::atan2(mat.m[1][0], mat.m[0][0]);
    } else{
        // 特異点の場合
        x = std::atan2(-mat.m[1][2], mat.m[1][1]);
        z = 0.0;
    }

    return {x, y, z}; // ラジアンで返す
}

Vector3 TransformVector(const Vector3& vector, const Matrix4x4& matrix){
    Vector3 result = {0, 0, 0};

    // 同次座標系への変換
    // 変換行列を適用
    Vector4 homogeneousCoordinate(
        vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0],
        vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1],
        vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2],
        vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3]);

    // 同次座標系から3次元座標系に戻す
    float w = homogeneousCoordinate.w;
    result.x = homogeneousCoordinate.x / w;
    result.y = homogeneousCoordinate.y / w;
    result.z = homogeneousCoordinate.z / w;

    return result;
}

float LengthSquared(const Vector3& v){
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

std::string GenerateGUID(){
    uint64_t high = MyFunc::RandomU64(); // 上位64ビット
    uint64_t low = MyFunc::RandomU64(); // 下位64ビット

    // "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" 形式にする
    std::ostringstream oss;
    oss << std::hex << std::nouppercase
        << std::setw(8) << std::setfill('0') << static_cast< unsigned int >((high >> 32) & 0xffffffff)
        << '-'
        << std::setw(4) << std::setfill('0') << static_cast< unsigned int >((high >> 16) & 0xffff)
        << '-'
        << std::setw(4) << std::setfill('0') << static_cast< unsigned int >(high & 0xffff)
        << '-'
        << std::setw(4) << std::setfill('0') << static_cast< unsigned int >((low >> 48) & 0xffff)
        << '-'
        << std::setw(4) << std::setfill('0') << static_cast< unsigned int >((low >> 32) & 0xffff)
        << std::setw(8) << std::setfill('0') << static_cast< unsigned int >(low & 0xffffffff);

    return oss.str();
}
