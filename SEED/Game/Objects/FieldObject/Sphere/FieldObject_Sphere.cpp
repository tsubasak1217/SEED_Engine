#include "FieldObject_Sphere.h"

///////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
///////////////////////////////////////////////////////////////////
FieldObject_Sphere::FieldObject_Sphere(){
    className_ = "FieldObject_Sphere";
    name_ = "sphere";
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
}

FieldObject_Sphere::FieldObject_Sphere(const std::string& modelName){
    className_ = "FieldObject_Sphere";
    name_ = modelName;
    std::string path = "FieldObject/" + modelName;
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
}

///////////////////////////////////////////////////////////////////
// 初期化関数
///////////////////////////////////////////////////////////////////
void FieldObject_Sphere::Initialize(){}