#pragma once

// engine

//local
#include "../FieldObject/FieldObject.h"
#include "../FieldObject/FieldObjectName.h"

// lib
#include "../lib/patterns/ISubject.h"
#include <vector>
#include <memory>

class Player;

class FieldObjectManager{
public:
    FieldObjectManager(ISubject& subject) : subject_(subject){}

    void Update();
    void Draw();
    void BeginFrame();
    void EndFrame();

    // すべてのオブジェクトをクリア
    void ClearAllFieldObjects();

    // 新しいオブジェクトを追加
    void AddFieldObject(std::unique_ptr<FieldObject> obj);
    void RemoveFieldObject(FieldObject* obj);

    // CollisionManagerにコライダーを渡す
    void HandOverColliders();

public:
    Vector3 GetStartPosition()const;

    std::vector<std::unique_ptr<FieldObject>>& GetObjects(){ return fieldObjects_; }

    template <typename T>
    std::vector<T*> GetObjectsOfType();
private:
    std::vector<std::unique_ptr<FieldObject>> fieldObjects_;
    ISubject& subject_;

    Player* player_ = nullptr;
};

////////////////////////////////////////////////////////////////////
//  テンプレート関数
////////////////////////////////////////////////////////////////////
template<typename T>
inline std::vector<T*> FieldObjectManager::GetObjectsOfType(){
    std::vector<T*> result;
    for (auto& objPtr : fieldObjects_){
        if (auto* casted = dynamic_cast< T* >(objPtr.get())){
            result.push_back(casted);
        }
    }
    return result;
}