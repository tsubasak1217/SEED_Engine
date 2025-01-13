#include "FieldObject_GroundCube.h"

/////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
/////////////////////////////////////////////////////////////////////
FieldObject_GroundCube::FieldObject_GroundCube(){
    className_ = "FieldObject_GroundCube";
    name_ = "groundCube";
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
}

FieldObject_GroundCube::FieldObject_GroundCube(const std::string& modelName){
    className_ = "FieldObject_GroundCube";
    name_ = modelName;
    std::string path = "FieldObject/" + modelName;
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
}


/////////////////////////////////////////////////////////////////////
// 初期化関数
/////////////////////////////////////////////////////////////////////
void FieldObject_GroundCube::Initialize(){}
