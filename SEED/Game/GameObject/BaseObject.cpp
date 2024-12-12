#include "BaseObject.h"

#include "Model.h"

BaseObject::BaseObject(){}

BaseObject::BaseObject(const std::string& _name)
    :name_(_name){}

BaseObject::~BaseObject(){}

void BaseObject::Draw(){
    model_->Draw();
}