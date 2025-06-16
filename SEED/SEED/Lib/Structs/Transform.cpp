#include "Transform.h"
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>

//////////////////////////////////////////////////////////////
// 行列に変換する関数
//////////////////////////////////////////////////////////////
Matrix3x3 Transform2D::ToMatrix(){
    return AffineMatrix(scale, rotate, translate);
}

Matrix4x4 Transform2D::ToMatrix4x4(){
    return AffineMatrix(
        { scale.x, scale.y, 1.0f },
        { 0.0f,0.0f,rotate },
        { translate.x,translate.y,0.0f }
    );
}

Matrix4x4 Transform::ToMatrix(){
    return AffineMatrix(scale, rotate, translate);
}

/////////////////////////////////////////////////////////////
// 行列からTransformに変換する関数
/////////////////////////////////////////////////////////////
void Transform2D::FromMatrix4x4(const Matrix4x4& mat){
    // 平行移動成分を抽出
    Vector3 translate3D = ExtractTranslation(mat);
    translate = { translate3D.x, translate3D.y }; // 2DなのでX,Y成分のみ使用
    // 拡大縮小成分を抽出
    Vector3 scale3D = ExtractScale(mat);
    scale = { scale3D.x, scale3D.y }; // 2DなのでX,Y成分のみ使用
    // 回転成分を抽出
    rotate = ExtractRotation(mat).z; // 2DなのでZ軸の回転のみ使用
}

void Transform::FromMatrix(const Matrix4x4& mat){
    // 平行移動成分を抽出
    translate = ExtractTranslation(mat);
    // 拡大縮小成分を抽出
    scale = ExtractScale(mat);
    // 回転成分を抽出
    rotate = Quaternion::ToQuaternion(ExtractRotation(mat));
}


