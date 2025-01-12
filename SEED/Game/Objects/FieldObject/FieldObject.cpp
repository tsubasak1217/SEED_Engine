#include "FieldObject.h"

FieldObject::FieldObject(const std::string& modelName)
    :name_(modelName){
    std::string path = "FieldObject/" + modelName;
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
}

void FieldObject::Update(){
    model_->Update();
}

void FieldObject::Draw(){
    model_->Draw();
}
