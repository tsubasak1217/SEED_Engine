#include "BaseObject.h"

#include "Model.h"

BaseObject::BaseObject(){}

BaseObject::BaseObject(const std::string& _name)
    :name_(_name){}

BaseObject::~BaseObject(){}

void BaseObject::Draw(){
    model_->Draw();
}

const Vector3& BaseObject::GetScale() const{
    return model_->scale_;
}

const Vector3& BaseObject::GetRotate() const{
    return model_->rotate_;
}

const Quaternion& BaseObject::GetQuaternion() const{
    return model_->rotateQuat_;
}

const Vector3& BaseObject::GetTranslate() const{
    return model_->translate_;
}

Vector3 BaseObject::GetWorldPos() const{
    Matrix4x4 worldMat = model_->GetWorldMat();
    return  {worldMat.m[3][0],worldMat.m[3][1],worldMat.m[3][2]};
}

void BaseObject::SetScale(const Vector3& scale){
    model_->scale_ = scale;
}

void BaseObject::SetRotate(const Vector3& rotate){
    model_->rotate_ = rotate;
}

void BaseObject::SetQuaternion(const Quaternion& quaternion){
    model_->rotateQuat_ = quaternion;
}

void BaseObject::SetTranslate(const Vector3& translate){
    model_->translate_ = translate;
}

// x, y, z (クォータニオンはwも) の個別セッター
void BaseObject::SetScaleX(float x){
    model_->scale_.x = x;
}

void BaseObject::SetScaleY(float y){
    model_->scale_.y = y;
}

void BaseObject::SetScaleZ(float z){
    model_->scale_.z = z;
}

void BaseObject::SetRotateX(float x){
    model_->rotate_.x = x;
}

void BaseObject::SetRotateY(float y){
    model_->rotate_.y = y;
}

void BaseObject::SetRotateZ(float z){
    model_->rotate_.z = z;
}

void BaseObject::SetQuaternionX(float x){
    model_->rotateQuat_.x = x;
}

void BaseObject::SetQuaternionY(float y){
    model_->rotateQuat_.y = y;
}

void BaseObject::SetQuaternionZ(float z){
    model_->rotateQuat_.z = z;
}

void BaseObject::SetQuaternionW(float w){
    model_->rotateQuat_.w = w;
}

void BaseObject::SetTranslateX(float x){
    model_->translate_.x = x;
}

void BaseObject::SetTranslateY(float y){
    model_->translate_.y = y;
}

void BaseObject::SetTranslateZ(float z){
    model_->translate_.z = z;
}
