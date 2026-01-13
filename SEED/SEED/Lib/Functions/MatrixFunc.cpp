#include <SEED/Lib/Functions/MatrixFunc.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <cmath>
#include <numbers>
#include <cassert>

namespace MatFunc {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     加算
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 加算
Matrix2x2 Add(const Matrix2x2& matrix1, const Matrix2x2& matrix2){

    Matrix2x2 result;

    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++){
            result.m[i][j] = matrix1.m[i][j] + matrix2.m[i][j];
        }
    }

    return result;
};
Matrix3x3 Add(const Matrix3x3& matrix1, const Matrix3x3& matrix2){

    Matrix3x3 result;

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            result.m[i][j] = matrix1.m[i][j] + matrix2.m[i][j];
        }
    }

    return result;
}

Matrix4x4 Add(const Matrix4x4& matrix1, const Matrix4x4& matrix2){

    Matrix4x4 result;

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            result.m[i][j] = matrix1.m[i][j] + matrix2.m[i][j];
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     減算
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 減算
Matrix2x2 Subtract(const Matrix2x2& matrix1, const Matrix2x2& matrix2){

    Matrix2x2 result;

    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++){
            result.m[i][j] = matrix1.m[i][j] - matrix2.m[i][j];
        }
    }

    return result;
};
Matrix3x3 Subtract(const Matrix3x3& matrix1, const Matrix3x3& matrix2){

    Matrix3x3 result;

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            result.m[i][j] = matrix1.m[i][j] - matrix2.m[i][j];
        }
    }

    return result;
};

Matrix4x4 Subtract(const Matrix4x4& matrix1, const Matrix4x4& matrix2){

    Matrix4x4 result;

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            result.m[i][j] = matrix1.m[i][j] - matrix2.m[i][j];
        }
    }

    return result;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     割り算
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// -----------------割り算------------------
Matrix2x2 Devide(const Matrix2x2& matrix, float devideNum){
    Matrix2x2 result;

    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++){
            result.m[i][j] = matrix.m[i][j] / devideNum;
        }
    }

    return result;
};

Matrix3x3 Devide(const Matrix3x3& matrix, float devideNum){

    Matrix3x3 result;

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            result.m[i][j] = matrix.m[i][j] / devideNum;
        }
    }

    return result;
}

Matrix4x4 Devide(const Matrix4x4& matrix, float devideNum){

    Matrix4x4 result;

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            result.m[i][j] = matrix.m[i][j] / devideNum;
        }
    }

    return result;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     行列の席を求める関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ---------------積を求める----------------
Vector2 Multiply(const Vector2& vector, const Matrix2x2& matrix){

    return {
        vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0],
        vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1]
    };
};
Vector2 Multiply(const Vector2& vector, const Matrix3x3& matrix){
    Vector2 result;
    float w;

    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
    w = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + 1.0f * matrix.m[2][2];

    if(w == 0){
        return result;
    }

    result.x /= w;
    result.y /= w;

    return result;
}

Vector3 Multiply(const Vector3& vector, const Matrix4x4& matrix){

    Vector3 result;
    float w;

    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
    result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
    w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

    assert(w != 0);

    result.x /= w;
    result.y /= w;
    result.z /= w;

    return result;

};
;

Matrix2x2 Multiply(const Matrix2x2& matrix1, const Matrix2x2& matrix2){

    Matrix2x2 result;

    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++){

            result.m[i][j] =
                (matrix1.m[i][0] * matrix2.m[0][j]) +
                (matrix1.m[i][1] * matrix2.m[1][j]);
        }
    }

    return result;
};
Matrix3x3 Multiply(const Matrix3x3& matrix1, const Matrix3x3& matrix2){

    Matrix3x3 result;

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){

            result.m[i][j] =
                (matrix1.m[i][0] * matrix2.m[0][j]) +
                (matrix1.m[i][1] * matrix2.m[1][j]) +
                (matrix1.m[i][2] * matrix2.m[2][j]);
        }
    }

    return result;
}

Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b){

    Matrix4x4 r;

    r.m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0] + a.m[0][3] * b.m[3][0];
    r.m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1] + a.m[0][3] * b.m[3][1];
    r.m[0][2] = a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][2];
    r.m[0][3] = a.m[0][0] * b.m[0][3] + a.m[0][1] * b.m[1][3] + a.m[0][2] * b.m[2][3] + a.m[0][3] * b.m[3][3];

    r.m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0] + a.m[1][3] * b.m[3][0];
    r.m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1] + a.m[1][3] * b.m[3][1];
    r.m[1][2] = a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][2];
    r.m[1][3] = a.m[1][0] * b.m[0][3] + a.m[1][1] * b.m[1][3] + a.m[1][2] * b.m[2][3] + a.m[1][3] * b.m[3][3];

    r.m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0] + a.m[2][3] * b.m[3][0];
    r.m[2][1] = a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1] + a.m[2][3] * b.m[3][1];
    r.m[2][2] = a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][2];
    r.m[2][3] = a.m[2][0] * b.m[0][3] + a.m[2][1] * b.m[1][3] + a.m[2][2] * b.m[2][3] + a.m[2][3] * b.m[3][3];

    r.m[3][0] = a.m[3][0] * b.m[0][0] + a.m[3][1] * b.m[1][0] + a.m[3][2] * b.m[2][0] + a.m[3][3] * b.m[3][0];
    r.m[3][1] = a.m[3][0] * b.m[0][1] + a.m[3][1] * b.m[1][1] + a.m[3][2] * b.m[2][1] + a.m[3][3] * b.m[3][1];
    r.m[3][2] = a.m[3][0] * b.m[0][2] + a.m[3][1] * b.m[1][2] + a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][2];
    r.m[3][3] = a.m[3][0] * b.m[0][3] + a.m[3][1] * b.m[1][3] + a.m[3][2] * b.m[2][3] + a.m[3][3] * b.m[3][3];

    return r;

    //Matrix4x4 result;

    //for(int i = 0; i < 4; i++){
    //    for(int j = 0; j < 4; j++){

    //        result.m[i][j] =
    //            (matrix1.m[i][0] * matrix2.m[0][j]) +
    //            (matrix1.m[i][1] * matrix2.m[1][j]) +
    //            (matrix1.m[i][2] * matrix2.m[2][j]) +
    //            (matrix1.m[i][3] * matrix2.m[3][j]);
    //    }
    //}

    //return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     行列をスカラー倍する関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// --------------スカラー倍----------------
Vector2 Multiply(const Vector2& vector, float scalar){
    return { vector.x * scalar,vector.y * scalar };
}
Vector3 Multiply(const Vector3& vector, float scalar){
    return { vector.x * scalar,vector.y * scalar,vector.z * scalar };
}
Matrix2x2 Multiply(const Matrix2x2& matrix, float scalar){

    Matrix2x2 result;
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++){
            result.m[i][j] = matrix.m[i][j] * scalar;
        }
    }

    return result;
}
Matrix3x3 Multiply(const Matrix3x3& matrix, float scalar){

    Matrix3x3 result;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            result.m[i][j] = matrix.m[i][j] * scalar;
        }
    }

    return result;
}
Matrix4x4 Multiply(const Matrix4x4& matrix, float scalar){

    Matrix4x4 result;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            result.m[i][j] = matrix.m[i][j] * scalar;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     変換関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 同時座標系からデカルト座標系に変換する関数
Vector2 TransformDescartes(const Vector2& vector, const Matrix3x3& matrix){
    Vector2 result;
    float w;

    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + matrix.m[2][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + matrix.m[2][1];
    w = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + matrix.m[2][2];

    assert(w != 0);

    result.x /= w;
    result.y /= w;

    return result;
}

Vector3 TransformDescartes(const Vector3& vector, const Matrix4x4& matrix){
    Vector3 result;
    float w;

    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
    result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
    w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

    assert(w != 0);

    result.x /= w;
    result.y /= w;
    result.z /= w;

    return result;
}

Vector3 TransformToVec3(const Vector2& vec){
    return Vector3(vec.x, vec.y, 1.0f);
}

Vector3 TransformToVec3(const Vector4& vec){
    assert(vec.w != 0.0f);
    return { vec.x / vec.w,vec.y / vec.w ,vec.z / vec.w };
}

Vector3 Expand0ToVec3(const Vector2& vec){
    return Vector3(vec.x, vec.y, 0.0f);
}

Vector3 Expand1ToVec3(const Vector2& vec){
    return Vector3(vec.x, vec.y, 0.0f);
}

Vector4 TransformToVec4(const Vector3& vec){
    return Vector4(vec.x, vec.y, vec.z, 1.0f);
}

Vector4 TransformToVec4(const Vector2& vec){
    return Vector4(vec.x, vec.y, 0.0f, 1.0f);
}

Matrix4x4 ToMat4x4(const Matrix3x3& mat){
    Matrix4x4 result = IdentityMat4();

    // scale,rotate成分
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++){
            result.m[i][j] = mat.m[i][j];
        }
    }
    // translate成分
    result.m[3][0] = mat.m[2][0];
    result.m[3][1] = mat.m[2][1];

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     単位行列を作る関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 単位行列を返す関数
Matrix2x2 IdentityMat2(){

    Matrix2x2 identity(
        1.0f, 0.0f,
        0.0f, 1.0f
    );

    return identity;
}

Matrix3x3 IdentityMat3(){

    Matrix3x3 identity(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    );

    return identity;
}

Matrix4x4 IdentityMat4(){

    Matrix4x4 identity(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    return identity;
}

Matrix4x4 NegaIdentityMat4(){

    Matrix4x4 identity(
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f
    );

    return identity;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     拡大縮小行列を作る関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------- 拡大縮小行列を作る関数 --------------------------*/
Matrix3x3 ScaleMatrix(float scaleX, float scaleY){
    Matrix3x3 matrix;
    matrix.m[0][0] = scaleX;
    matrix.m[0][1] = 0;
    matrix.m[0][2] = 0;

    matrix.m[1][0] = 0;
    matrix.m[1][1] = scaleY;
    matrix.m[1][2] = 0;

    matrix.m[2][0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][2] = 1;

    return matrix;
}

Matrix3x3 ScaleMatrix(const Vector2& scale){
    Matrix3x3 matrix;
    matrix.m[0][0] = scale.x;
    matrix.m[0][1] = 0;
    matrix.m[0][2] = 0;

    matrix.m[1][0] = 0;
    matrix.m[1][1] = scale.y;
    matrix.m[1][2] = 0;

    matrix.m[2][0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][2] = 1;

    return matrix;
}

Matrix4x4 ScaleMatrix(float scaleX, float scaleY, float scaleZ){
    Matrix4x4 matrix;
    matrix.m[0][0] = scaleX;
    matrix.m[0][1] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][3] = 0;

    matrix.m[1][0] = 0;
    matrix.m[1][1] = scaleY;
    matrix.m[1][2] = 0;
    matrix.m[1][3] = 0;

    matrix.m[2][0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][2] = scaleZ;
    matrix.m[2][3] = 0;

    matrix.m[3][0] = 0;
    matrix.m[3][1] = 0;
    matrix.m[3][2] = 0;
    matrix.m[3][3] = 1;

    return matrix;
}

Matrix4x4 ScaleMatrix(const Vector3& scale){
    Matrix4x4 matrix;
    matrix.m[0][0] = scale.x;
    matrix.m[0][1] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][3] = 0;

    matrix.m[1][0] = 0;
    matrix.m[1][1] = scale.y;
    matrix.m[1][2] = 0;
    matrix.m[1][3] = 0;

    matrix.m[2][0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][2] = scale.z;
    matrix.m[2][3] = 0;

    matrix.m[3][0] = 0;
    matrix.m[3][1] = 0;
    matrix.m[3][2] = 0;
    matrix.m[3][3] = 1;

    return matrix;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     回転行列を作る関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*------------------------- 回転行列を作る関数 --------------------------*/
Matrix3x3 RotateMatrix(float theta){

    Matrix3x3 result;
    result.m[0][0] = std::cos(theta);
    result.m[0][1] = std::sin(theta);
    result.m[0][2] = 0.0f;

    result.m[1][0] = -std::sin(theta);
    result.m[1][1] = std::cos(theta);
    result.m[1][2] = 0.0f;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1;

    return result;
}

Matrix4x4 RotateXMatrix(float theta){
    Matrix4x4 mat = IdentityMat4();
    float sin = std::sin(theta);
    float cos = std::cos(theta);
    mat.m[1][1] = cos;
    mat.m[2][1] = -sin;
    mat.m[1][2] = sin;
    mat.m[2][2] = cos;
    return mat;
}

Matrix4x4 RotateYMatrix(float theta){
    Matrix4x4 mat = IdentityMat4();
    float sin = std::sin(theta);
    float cos = std::cos(theta);
    mat.m[0][0] = cos;
    mat.m[2][0] = sin;
    mat.m[0][2] = -sin;
    mat.m[2][2] = cos;
    return mat;
}

Matrix4x4 RotateZMatrix(float theta){
    Matrix4x4 mat = IdentityMat4();
    float sin = std::sin(theta);
    float cos = std::cos(theta);
    mat.m[0][0] = cos;
    mat.m[1][0] = -sin;
    mat.m[0][1] = sin;
    mat.m[1][1] = cos;
    return mat;
}

Matrix4x4 RotateMatrix(const Vector3& rotate){
    Matrix4x4 rotX = std::abs(rotate.x) > 1e-6f ? RotateXMatrix(rotate.x) : IdentityMat4();
    Matrix4x4 rotY = std::abs(rotate.y) > 1e-6f ? RotateYMatrix(rotate.y) : IdentityMat4();
    Matrix4x4 rotZ = std::abs(rotate.z) > 1e-6f ? RotateZMatrix(rotate.z) : IdentityMat4();
    return rotX * (rotY * rotZ);
}

/*------------------- Quaternionから回転行列を作成 --------------------*/
Matrix4x4 RotateMatrix(const Quaternion& rotate){
    return rotate.MakeMatrix();
}

/*------------------- 任意軸と角度から回転行列を作成 --------------------*/
Matrix4x4 RotateMatrix(const Vector3& axis, float angle){
    Quaternion q = Quaternion::AngleAxis(angle, axis);
    return q.MakeMatrix();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     平行移動行列を作る関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Matrix3x3 TranslateMatrix(float tx, float ty){
    Matrix3x3 matrix;
    matrix.m[0][0] = 1;  matrix.m[0][1] = 0;  matrix.m[0][2] = 0;
    matrix.m[1][0] = 0;  matrix.m[1][1] = 1;  matrix.m[1][2] = 0;
    matrix.m[2][0] = tx; matrix.m[2][1] = ty; matrix.m[2][2] = 1;
    return matrix;
}

Matrix3x3 TranslateMatrix(const Vector2& t){
    Matrix3x3 matrix;
    matrix.m[0][0] = 1;   matrix.m[0][1] = 0;   matrix.m[0][2] = 0;
    matrix.m[1][0] = 0;   matrix.m[1][1] = 1;   matrix.m[1][2] = 0;
    matrix.m[2][0] = t.x; matrix.m[2][1] = t.y; matrix.m[2][2] = 1;
    return matrix;
}

Matrix4x4 TranslateMatrix(float tx, float ty, float tz){
    Matrix4x4 matrix;
    matrix.m[0][0] = 1;  matrix.m[0][1] = 0;  matrix.m[0][2] = 0;  matrix.m[0][3] = 0;
    matrix.m[1][0] = 0;  matrix.m[1][1] = 1;  matrix.m[1][2] = 0;  matrix.m[1][3] = 0;
    matrix.m[2][0] = 0;  matrix.m[2][1] = 0;  matrix.m[2][2] = 1;  matrix.m[2][3] = 0;
    matrix.m[3][0] = tx; matrix.m[3][1] = ty; matrix.m[3][2] = tz; matrix.m[3][3] = 1;
    return matrix;
}

Matrix4x4 TranslateMatrix(const Vector3& t){
    Matrix4x4 matrix;
    matrix.m[0][0] = 1;   matrix.m[0][1] = 0;   matrix.m[0][2] = 0;   matrix.m[0][3] = 0;
    matrix.m[1][0] = 0;   matrix.m[1][1] = 1;   matrix.m[1][2] = 0;   matrix.m[1][3] = 0;
    matrix.m[2][0] = 0;   matrix.m[2][1] = 0;   matrix.m[2][2] = 1;   matrix.m[2][3] = 0;
    matrix.m[3][0] = t.x; matrix.m[3][1] = t.y; matrix.m[3][2] = t.z; matrix.m[3][3] = 1;
    return matrix;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     アフィン変換行列を作る関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Matrix3x3 AffineMatrix(Vector2 scale, float rotateTheta, Vector2 translate){

    Matrix3x3 matrix;
    matrix.m[0][0] = scale.x * cosf(rotateTheta);
    matrix.m[0][1] = scale.x * sinf(rotateTheta);
    matrix.m[0][2] = 0;

    matrix.m[1][0] = scale.y * -sinf(rotateTheta);
    matrix.m[1][1] = scale.y * cosf(rotateTheta);
    matrix.m[1][2] = 0;

    matrix.m[2][0] = translate.x;
    matrix.m[2][1] = translate.y;
    matrix.m[2][2] = 1;

    return matrix;
}

Matrix3x3 AffineMatrix(const Transform2D& transform){
    return AffineMatrix(transform.scale, transform.rotate, transform.translate);
}

Matrix4x4 AffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate){
    Matrix4x4 matrix(Multiply(ScaleMatrix(scale), RotateMatrix(rotate)));
    matrix.m[3][0] = translate.x;
    matrix.m[3][1] = translate.y;
    matrix.m[3][2] = translate.z;
    return matrix;
}

Matrix4x4 AffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate){
    Matrix4x4 matrix(Multiply(ScaleMatrix(scale), rotate.MakeMatrix()));
    matrix.m[3][0] = translate.x;
    matrix.m[3][1] = translate.y;
    matrix.m[3][2] = translate.z;
    return matrix;
}

Matrix4x4 AffineMatrix(const Transform& transform){
    return AffineMatrix(transform.scale, transform.rotate, transform.translate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     ワールド行列から各SRT成分を取り出す関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vector2 ExtractTranslation(const Matrix3x3& matrix){
    return Vector2(matrix.m[2][0], matrix.m[2][1]);
}

// 行列から平行移動成分を取り出す
Vector3 ExtractTranslation(const Matrix4x4& matrix){
    return Vector3(matrix.m[3][0], matrix.m[3][1], matrix.m[3][2]);
}

Vector2 ExtractScale(const Matrix3x3& matrix){
    float scaleX = MyMath::Length(Vector2(matrix.m[0][0], matrix.m[0][1]));
    float scaleY = MyMath::Length(Vector2(matrix.m[1][0], matrix.m[1][1]));
    return Vector2(scaleX, scaleY);
}

// 行列から拡大縮小成分を取り出す
Vector3 ExtractScale(const Matrix4x4& matrix){
    float scaleX = MyMath::Length(Vector3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2]));
    float scaleY = MyMath::Length(Vector3(matrix.m[1][0], matrix.m[1][1], matrix.m[1][2]));
    float scaleZ = MyMath::Length(Vector3(matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]));
    return Vector3(scaleX, scaleY, scaleZ);
}

float ExtractRotation(const Matrix3x3& matrix){
    // スケールを取り除くために各軸を正規化
    Vector2 scale = ExtractScale(matrix);
    Matrix3x3 rotationMatrix = matrix;
    rotationMatrix.m[0][0] /= (scale.x != 0.0f) ? scale.x : 1e-10f;
    rotationMatrix.m[0][1] /= (scale.x != 0.0f) ? scale.x : 1e-10f;
    rotationMatrix.m[1][0] /= (scale.y != 0.0f) ? scale.y : 1e-10f;
    rotationMatrix.m[1][1] /= (scale.y != 0.0f) ? scale.y : 1e-10f;
    // 回転角を抽出
    return atan2(rotationMatrix.m[0][1], rotationMatrix.m[0][0]);
}

// 行列から回転成分を取り出す
Vector3 ExtractRotation(const Matrix4x4& matrix){
    // スケールを取り除くために各軸を正規化
    Vector3 scale = ExtractScale(matrix);
    Matrix4x4 rotationMatrix = matrix;

    rotationMatrix.m[0][0] /= (scale.x != 0.0f) ? scale.x : 1e-10f;
    rotationMatrix.m[0][1] /= (scale.x != 0.0f) ? scale.x : 1e-10f;
    rotationMatrix.m[0][2] /= (scale.x != 0.0f) ? scale.x : 1e-10f;

    rotationMatrix.m[1][0] /= (scale.y != 0.0f) ? scale.y : 1e-10f;
    rotationMatrix.m[1][1] /= (scale.y != 0.0f) ? scale.y : 1e-10f;
    rotationMatrix.m[1][2] /= (scale.y != 0.0f) ? scale.y : 1e-10f;

    rotationMatrix.m[2][0] /= (scale.z != 0.0f) ? scale.z : 1e-10f;
    rotationMatrix.m[2][1] /= (scale.z != 0.0f) ? scale.z : 1e-10f;
    rotationMatrix.m[2][2] /= (scale.z != 0.0f) ? scale.z : 1e-10f;

    // オイラー角を抽出 (YXZ順)
    float pitch, yaw, roll;

    if(rotationMatrix.m[2][1] < 1){
        if(rotationMatrix.m[2][1] > -1){
            yaw = asin(-rotationMatrix.m[2][1]);
            pitch = atan2(rotationMatrix.m[2][0], rotationMatrix.m[2][2]);
            roll = atan2(rotationMatrix.m[0][1], rotationMatrix.m[1][1]);
        } else{
            // 特殊ケース: rotationMatrix.m[2][1] = -1
            yaw = std::numbers::pi_v<float> / 2;
            pitch = -atan2(-rotationMatrix.m[1][0], rotationMatrix.m[0][0]);
            roll = 0;
        }
    } else{
        // 特殊ケース: rotationMatrix.m[2][1] = 1
        yaw = -std::numbers::pi_v<float> / 2;
        pitch = atan2(-rotationMatrix.m[1][0], rotationMatrix.m[0][0]);
        roll = 0;
    }

    return Vector3(yaw, pitch, roll);
}

Quaternion ExtractQuaternion(const Matrix4x4& matrix){
    return Quaternion::MatrixToQuaternion(matrix);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    　逆行列を求める関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 正則行列かどうか確認する関数--------------------------------------
// 3x3 行列の行列式を計算する関数
float Determinant3x3(const Matrix3x3& mat){
    return
        (mat.m[0][0] * mat.m[1][1] * mat.m[2][2]) +
        (mat.m[0][1] * mat.m[1][2] * mat.m[2][0]) +
        (mat.m[0][2] * mat.m[1][0] * mat.m[2][1]) -
        (mat.m[0][2] * mat.m[1][1] * mat.m[2][0]) -
        (mat.m[0][1] * mat.m[1][0] * mat.m[2][2]) -
        (mat.m[0][0] * mat.m[1][2] * mat.m[2][1]);
}

// 4x4 行列の行列式を計算する関数
float Determinant4x4(const Matrix4x4& mat){
    float det = 0.0f;
    for(int i = 0; i < 4; i++){

        Matrix3x3 minor;

        for(int j = 0; j < 3; j++){
            int k = 0;
            for(int l = 0; l < 4; l++){
                if(l != i){
                    minor.m[j][k++] = mat.m[j + 1][l];
                }
            }
        }

        float subDet = Determinant3x3(minor);
        det += (i % 2 == 0 ? 1.0f : -1.0f) * mat.m[0][i] * subDet;
    }
    return det;
}

//逆行列を求める関数
Matrix2x2 InverseMatrix(const Matrix2x2& matrix){

    float det = (matrix.m[0][0] * matrix.m[1][1]) - (matrix.m[0][1] * matrix.m[1][0]);
    assert(det != 0);


    Matrix2x2 result;
    result.m[0][0] = matrix.m[1][1];
    result.m[0][1] = -matrix.m[0][1];

    result.m[1][0] = -matrix.m[1][0];
    result.m[1][1] = matrix.m[0][0];

    return Devide(result, det);
};

Matrix3x3 InverseMatrix(const Matrix3x3& matrix){

    Matrix3x3 m = matrix;

    // スケール0対策（列ベクトル長が0に近い場合）
    const float eps = 1e-6f;

    // X方向
    float sx = std::sqrt(m.m[0][0] * m.m[0][0] + m.m[1][0] * m.m[1][0]);
    if(std::fabs(sx) < eps){
        sx = (sx < 0 ? -eps : eps);
        m.m[0][0] = sx;
    }

    // Y方向
    float sy = std::sqrt(m.m[0][1] * m.m[0][1] + m.m[1][1] * m.m[1][1]);
    if(std::fabs(sy) < eps){
        sy = (sy < 0 ? -eps : eps);
        m.m[1][1] = sy;
    }

    // 逆行列の計算
    float det =
        (m.m[0][0] * m.m[1][1] * m.m[2][2]) +
        (m.m[0][1] * m.m[1][2] * m.m[2][0]) +
        (m.m[0][2] * m.m[1][0] * m.m[2][1]) -
        (m.m[0][2] * m.m[1][1] * m.m[2][0]) -
        (m.m[0][1] * m.m[1][0] * m.m[2][2]) -
        (m.m[0][0] * m.m[1][2] * m.m[2][1]);

    if(std::fabs(det) < eps){
        det = (det < 0 ? -eps : eps);
    }

    m.m[0][0] = matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1];
    m.m[0][1] = -(matrix.m[0][1] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][1]);
    m.m[0][2] = matrix.m[0][1] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][1];

    m.m[1][0] = -(matrix.m[1][0] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][0]);
    m.m[1][1] = matrix.m[0][0] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][0];
    m.m[1][2] = -(matrix.m[0][0] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][0]);

    m.m[2][0] = matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0];
    m.m[2][1] = -(matrix.m[0][0] * matrix.m[2][1] - matrix.m[0][1] * matrix.m[2][0]);
    m.m[2][2] = matrix.m[0][0] * matrix.m[1][1] - matrix.m[0][1] * matrix.m[1][0];

    return Devide(m, det);
}

Matrix4x4 InverseMatrix(const Matrix4x4& matrix){

    Matrix4x4 inv;
    float sweep[4][8];

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            // sweepの左側に逆行列を求める行列をセット
            sweep[i][j] = matrix.m[i][j];

            // sweepの右半分は単位行列で初期化
            sweep[i][4 + j] = (i == j) ? 1.0f : 0.0f;
        }
    }

    // 左半分が単位行列になるまで繰り返す (右半分に逆行列が求められる)
    for(int col = 0; col < 4; col++){

        /*------------------------------------------------------*/
        /*				       	ソート、除外						*/
        /*------------------------------------------------------*/

        /* 最大の絶対値を注目対角成分の絶対値と仮定 */
        float max = fabs(sweep[col][col]);
        int maxIdx = col;

        // 今見ている対角成分より大きい絶対値を持つ要素がその列のその行より後にあるか探す
        for(int row = col + 1; row < 4; row++){
            if(fabs(sweep[row][col]) > max){
                max = fabs(sweep[row][col]);
                maxIdx = row;
            }
        }

        // 最大値が0の場合、逆行列は求められない
        if(fabs(sweep[maxIdx][col]) == 0){ return IdentityMat4(); }

        // 見つかった場合、その要素が見つかった行と今の行の要素を入れ替える
        if(col != maxIdx){
            for(int col2 = 0; col2 < 8; col2++){
                std::swap(sweep[maxIdx][col2], sweep[col][col2]);
            }
        }

        /*------------------------------------------------------*/
        /*				       	逆行列の計算						*/
        /*------------------------------------------------------*/

        /*--------- 今見ている列の対角成分を1にする ---------*/

        // 対角成分 sweep[col][col]に掛けると1になる値を求める
        float x = 1.0f / sweep[col][col];

        for(int col2 = 0; col2 < 8; col2++){
            // この計算でsweep[col][col]が1になる 
            // (対角成分以外にもその行すべての要素に掛ける。)
            sweep[col][col2] *= x;
        }

        /*------- 今見ている列の対角成分以外を0にする -------*/
        for(int row = 0; row < 4; row++){

            if(row == col){ continue; }// 対角成分はそのまま

            // 対角成分のある行以外に掛ける値を求める
            x = -sweep[row][col];

            for(int col2 = 0; col2 < 8; col2++){
                // 対角成分を1にした行をa倍して足していく
                // すると対角成分以外のsweep[row][col]が0になる ( 自分に対して 1 x -自分 を足しているため。)
                sweep[row][col2] += sweep[col][col2] * x;
            }
        }
    }

    // sweepの右半分がmatrixの逆行列
    for(int row = 0; row < 4; row++){
        for(int col = 0; col < 4; col++){
            inv.m[row][col] = sweep[row][4 + col];
        }
    }

    return inv;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    　転置行列を求める関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Matrix2x2 Transpose(const Matrix2x2& matrix){

    Matrix2x2 result;
    result.m[0][0] = matrix.m[0][0];
    result.m[0][1] = matrix.m[1][0];

    result.m[1][0] = matrix.m[0][1];
    result.m[1][1] = matrix.m[1][1];

    return result;
};

Matrix3x3 Transpose(const Matrix3x3& matrix){

    Matrix3x3 result;
    result.m[0][0] = matrix.m[0][0];
    result.m[0][1] = matrix.m[1][0];
    result.m[0][2] = matrix.m[2][0];

    result.m[1][0] = matrix.m[0][1];
    result.m[1][1] = matrix.m[1][1];
    result.m[1][2] = matrix.m[2][1];

    result.m[2][0] = matrix.m[0][2];
    result.m[2][1] = matrix.m[1][2];
    result.m[2][2] = matrix.m[2][2];

    return result;
}

Matrix4x4 Transpose(const Matrix4x4& matrix){
    Matrix4x4 result;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            result.m[i][j] = matrix.m[j][i];
        }
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    　投影行列を求める関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 正射影行列を求める関数(2D)
Matrix3x3 OrthoMatrix(float left, float right, float top, float bottom){

    Matrix3x3 result;

    result.m[0][0] = 2.0f / (right - left);
    result.m[0][1] = 0;
    result.m[0][2] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[1][2] = 0;

    result.m[2][0] = (left + right) / (left - right);
    result.m[2][1] = (top + bottom) / (bottom - top);
    result.m[2][2] = 1;

    return result;
}

// 正射影行列を求める関数(3D)
Matrix4x4 OrthoMatrix(float left, float right, float top, float bottom, float znear, float zfar){

    Matrix4x4 result;

    result.m[0][0] = 2.0f / (right - left);
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1.0f / (zfar - znear);
    result.m[2][3] = 0;

    result.m[3][0] = (left + right) / (left - right);
    result.m[3][1] = (top + bottom) / (bottom - top);
    result.m[3][2] = znear / (znear - zfar);
    result.m[3][3] = 1;

    return result;
}

// アスペクト比を求める関数
float AspectRatio(float windowWidth, float windowHeight){
    return windowWidth / windowHeight;
}

// 透視投影行列を求める関数
Matrix4x4 PerspectiveMatrix(float fovY, float aspectRatio, float znear, float zfar){

    Matrix4x4 result;

    result.m[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f));
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 1.0f / std::tan(fovY / 2.0f);
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = zfar / (zfar - znear);
    result.m[2][3] = 1;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = (-znear * zfar) / (zfar - znear);
    result.m[3][3] = 0;

    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    　ビューポート行列を求める関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//ビューポート変換行列を求める関数(2D)
Matrix3x3 ViewportMatrix(const Vector2& size, const Vector2& LeftTop){

    Matrix3x3 result;

    result.m[0][0] = size.x * 0.5f;
    result.m[0][1] = 0;
    result.m[0][2] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = -(size.y * 0.5f);
    result.m[1][2] = 0;

    result.m[2][0] = LeftTop.x + (size.x * 0.5f);
    result.m[2][1] = LeftTop.y + (size.y * 0.5f);
    result.m[2][2] = 1;

    return result;
}

// ビューポート変換行列を求める関数(3D)
Matrix4x4 ViewportMatrix(const Vector2& size, const Vector2& LeftTop, float minDepth, float maxDepth){

    Matrix4x4 result;

    result.m[0][0] = size.x * 0.5f;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = -(size.y * 0.5f);
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = maxDepth - minDepth;
    result.m[2][3] = 0;

    result.m[3][0] = LeftTop.x + (size.x * 0.5f);
    result.m[3][1] = LeftTop.y + (size.y * 0.5f);
    result.m[3][2] = minDepth;
    result.m[3][3] = 1;

    return result;
}
;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    　WvpVp行列を求める関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//レンダリングパイプライン作る関数
Matrix3x3 WvpVpMatrix(
    Vector2 playerPos, Vector2 playerScale, float playerAngle,
    Vector2 cameraPos, Vector2 cameraScale, float cameraAngle,
    Vector2 cameraRange, Vector2 leftTop,
    Vector2 screenSize
){

    Matrix3x3 worldMatrix;
    Matrix3x3 cameraMatrix;
    Matrix3x3 viewMatrix;
    Matrix3x3 orthoMatrix;
    Matrix3x3 viewPortMatrix;
    Matrix3x3 result;

    worldMatrix = AffineMatrix(playerScale, playerAngle, playerPos);//プレイヤーのワールド行列
    cameraMatrix = AffineMatrix(cameraScale, cameraAngle, cameraPos);
    viewMatrix = InverseMatrix(cameraMatrix);//カメラの逆行列
    orthoMatrix = OrthoMatrix(//正射影行列
        -(cameraRange.x * 0.5f),
        cameraRange.x * 0.5f,
        -(cameraRange.y * 0.5f),
        cameraRange.y * 0.5f
    );
    viewPortMatrix = ViewportMatrix(//ビューポート行列
        screenSize, leftTop
    );

    //行列の合成
    result = Multiply(worldMatrix, viewMatrix);
    result = Multiply(result, orthoMatrix);
    result = Multiply(result, viewPortMatrix);

    return result;
}

} // namespace MatFunc