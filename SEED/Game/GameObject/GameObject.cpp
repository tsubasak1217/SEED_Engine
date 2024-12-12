#include "GameObject.h"

#include "Model.h"

GameObject::GameObject(){}

GameObject::GameObject(const std::string& _name)
    :name_(_name){
    // model 読み込み
    model_ = std::make_unique<Model>("teapot.obj");
}

GameObject::~GameObject(){}

void GameObject::Draw(){
    model_->Draw();
}

const Vector3& GameObject::GetScale() const{
    return model_->scale_;
}

const Vector3& GameObject::GetRotate() const{
    return model_->rotate_;
}

const Quaternion& GameObject::GetQuaternion() const{
    return model_->rotateQuat_;
}

const Vector3& GameObject::GetTranslate() const{
    return model_->translate_;
}

Vector3 GameObject::GetWorldPos() const{
    Matrix4x4 worldMat = model_->GetWorldMat();
    return  {worldMat.m[3][0],worldMat.m[3][1],worldMat.m[3][2]};
}

void GameObject::SetScale(const Vector3& scale){
    model_->scale_ = scale;
}

void GameObject::SetRotate(const Vector3& rotate){
    model_->rotate_ = rotate;
}

void GameObject::SetQuaternion(const Quaternion& quaternion){
    model_->rotateQuat_ = quaternion;
}

void GameObject::SetTranslate(const Vector3& translate){
    model_->translate_ = translate;
}

// x, y, z (クォータニオンはwも) の個別セッター
void GameObject::SetScaleX(float x){
    model_->scale_.x = x;
}

void GameObject::SetScaleY(float y){
    model_->scale_.y = y;
}

void GameObject::SetScaleZ(float z){
    model_->scale_.z = z;
}

void GameObject::SetRotateX(float x){
    model_->rotate_.x = x;
}

void GameObject::SetRotateY(float y){
    model_->rotate_.y = y;
}

void GameObject::SetRotateZ(float z){
    model_->rotate_.z = z;
}

void GameObject::SetQuaternionX(float x){
    model_->rotateQuat_.x = x;
}

void GameObject::SetQuaternionY(float y){
    model_->rotateQuat_.y = y;
}

void GameObject::SetQuaternionZ(float z){
    model_->rotateQuat_.z = z;
}

void GameObject::SetQuaternionW(float w){
    model_->rotateQuat_.w = w;
}

void GameObject::SetTranslateX(float x){
    model_->translate_.x = x;
}

void GameObject::SetTranslateY(float y){
    model_->translate_.y = y;
}

void GameObject::SetTranslateZ(float z){
    model_->translate_.z = z;
}
