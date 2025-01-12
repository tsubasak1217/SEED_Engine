// FieldObjectManager.cpp

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
    // オブジェクトをクリアする前に、オブザーバー登録を解除
    for (auto& obj : fieldObjects_){
        IObserver* observer = dynamic_cast< IObserver* >(obj.get());
        if (observer){
            subject_.UnregisterObserver(observer);
        }
    }
    fieldObjects_.clear();
}

void FieldObjectManager::AddFieldObject(std::unique_ptr<FieldObject> obj){
    // オブジェクトが IObserver を実装している場合、EventManager に登録
    IObserver* observer = dynamic_cast< IObserver* >(obj.get());
    if (observer){
        subject_.RegisterObserver(observer);
    }

    fieldObjects_.push_back(std::move(obj));
}
