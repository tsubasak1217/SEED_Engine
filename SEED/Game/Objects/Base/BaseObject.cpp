#include "BaseObject.h"

uint32_t BaseObject::nextID_ = 0;

BaseObject::BaseObject(){
    objectID_ = nextID_++;
    name_ = "BaseObject";
    Initialize();
}

BaseObject::~BaseObject(){}

void BaseObject::Initialize(){
    model_ = std::make_unique<Model>("suzanne2.obj");
    model_->UpdateMatrix();
}

void BaseObject::Update(){
    model_->Update();
}

void BaseObject::Draw(){
    model_->Draw();
}
