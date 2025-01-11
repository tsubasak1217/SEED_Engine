#include "FieldObjectManager.h"

void FieldObjectManager::Update(){
    for (auto& fieldObject : fieldObjects_){
        fieldObject->Update();
    }
}

void FieldObjectManager::Draw(){
    for (auto& fieldObject : fieldObjects_){
        fieldObject->Draw();
    }
}

void FieldObjectManager::ClearAllFieldObjects(){
    fieldObjects_.clear();
}

void FieldObjectManager::AddFieldObject(std::unique_ptr<FieldObject> obj){
    fieldObjects_.push_back(std::move(obj));
}