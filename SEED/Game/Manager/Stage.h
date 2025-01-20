#pragma once

// engine

//local
#include "../FieldObject/FieldObject.h"
#include "../FieldObject/FieldObjectName.h"
#include "FieldObject/Goal/FieldObject_Goal.h"
#include "FieldObject/Start/FieldObject_Start.h"

// FieldObject
#include "FieldObject/Door/FieldObject_Door.h"
#include "FieldObject/GrassSoil/FieldObject_GrassSoil.h"
#include "FieldObject/Soil/FieldObject_Soil.h"
#include "FieldObject/Sphere/FieldObject_Sphere.h"
#include "FieldObject/Start/FieldObject_Start.h"
#include "FieldObject/Goal/FieldObject_Goal.h"
#include "FieldObject/Switch/FieldObject_Switch.h"
#include "FieldObject/ViewPoint/FieldObject_ViewPoint.h"

// lib
#include "../lib/patterns/ISubject.h"
#include <vector>
#include <memory>

class Stage{
public:
    Stage(ISubject& subject): subject_(subject){}

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

    // stageの読み込み
    void LoadFromJson(const std::string& filePath);
    void AddModel(
        uint32_t modelNameIndex,
        const Vector3& scale = {2.5f,2.5f,2.5f},
        const Vector3& rotate = {0.0f,0.0f,0.0f},
        const Vector3& translate = {0.0f,0.0f,0.0f}
    );

public:
    Vector3 GetStartPosition()const;
    FieldObject_ViewPoint* GetViewPoint()const;
    std::vector<std::unique_ptr<FieldObject>>& GetObjects(){ return fieldObjects_; }

    int GetStageNo()const{ return stageNo_; }
    void SetStageNo(int32_t stageNo){ stageNo_ = stageNo; }

    bool IsGoal()const{ 
        if(goalObject_){
            return goalObject_->isGoal_;
        }
        return false;
    }

    uint32_t GetDifficulty()const{ return difficulty_; }
    template <typename T>
    std::vector<T*> GetObjectsOfType();

private:
    int32_t stageNo_ = -1;
    uint32_t difficulty_ = 0;
    std::vector<std::unique_ptr<FieldObject>> fieldObjects_;

    //特殊処理 をするため ポインターを個別で保持
    FieldObject_Start* startObject_ = nullptr;
    FieldObject_Goal* goalObject_ = nullptr;

    ISubject& subject_;
};

////////////////////////////////////////////////////////////////////
//  テンプレート関数
////////////////////////////////////////////////////////////////////
template<typename T>
inline std::vector<T*> Stage::GetObjectsOfType(){
    std::vector<T*> result;
    for(auto& objPtr : fieldObjects_){
        if(auto* casted = dynamic_cast<T*>(objPtr.get())){
            result.push_back(casted);
        }
    }
    return result;
}