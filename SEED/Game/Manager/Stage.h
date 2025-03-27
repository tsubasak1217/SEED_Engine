#pragma once

// engine

//local
#include "../FieldObject/FieldObject.h"
#include "../FieldObject/FieldObjectName.h"
#include "FieldObject/Goal/FieldObject_Goal.h"
#include "FieldObject/Start/FieldObject_Start.h"

// FieldObject
#include "FieldObject/GrassSoil/FieldObject_GrassSoil.h"
#include "FieldObject/Soil/FieldObject_Soil.h"
#include "FieldObject/Star/FieldObject_Star.h"
#include "FieldObject/ViewPoint/FieldObject_ViewPoint.h"
#include "FieldObject/EventArea/FieldObject_EventArea.h"
#include "FieldObject/PointLight/FieldObject_PointLight.h"
#include "FieldObject/Plant/FieldObject_Plant.h"
#include "FieldObject/Wood/FieldObject_Wood.h"
#include "FieldObject/Fence/FieldObject_Fence.h"
#include "FieldObject/Tile/FieldObject_Tile.h"
#include "FieldObject/Box/FieldObject_Box.h"
#include "FieldObject/Chikuwa/FieldObject_Chikuwa.h"
#include "FieldObject/SaveArea/FieldObject_SaveArea.h"
#include "FieldObject/CameraControlArea/FieldObject_CameraControlArea.h"

// lib
#include <vector>
#include <memory>

class Player;

class Stage{
public:
    Stage(uint32_t stageNo);

    // 卵の取得数など,ステータスの初期化(ステージを遷移したときなどに呼ぶ)
    void InitializeStatus(bool isSaveData = false);
    void InitializeStatus(const std::string& _jsonFilePath);

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
        const nlohmann::json& json,
        const Vector3& scale = {2.5f,2.5f,2.5f},
        const Vector3& rotate = {0.0f,0.0f,0.0f},
        const Vector3& translate = {0.0f,0.0f,0.0f}
    );

    void UpdateStarCount();
public:
    Vector3 GetStartPosition()const;
    FieldObject_Goal* GetGoalObject()const;
    FieldObject_ViewPoint* GetViewPoint()const;
    std::vector<std::unique_ptr<FieldObject>>& GetObjects(){ return fieldObjects_; }

    FieldObject* GetSelectedObject()const{ return selectedObject_; }
    void SetSelectedObject(FieldObject* obj){ selectedObject_ = obj; }
    // 指定されたGUIDのオブジェクトを取得
    FieldObject* GetFieldObjectByGUID(const std::string& guid) const{
        for(const auto& objPtr : fieldObjects_){
            if(objPtr->GetGUID() == guid){
                return objPtr.get();
            }
        }
        return nullptr;
    }
    // 選択オブジェクトのGUIDを設定
    void SetSelectedObjectGUID(const std::string& guid){ selectedObjectGUID_ = guid; }
    // 選択オブジェクトのGUIDを取得
    const std::string& GetSelectedObjectGUID() const{ return selectedObjectGUID_; }
    int GetStageNo()const{ return stageNo_; }
    void SetStageNo(int32_t stageNo){ stageNo_ = stageNo; }
    bool IsGoal()const{
        if(goalObject_){
            return goalObject_->isGoal_;
        }
        return false;
    }
    uint32_t GetDifficulty()const{ return difficulty_; }
    void SetDifficulty(uint32_t difficulty){ difficulty_ = difficulty; }
    template <typename T>
    std::vector<T*> GetObjectsOfType();


private:
    int32_t stageNo_ = -1;
    uint32_t difficulty_ = 0;
    std::vector<std::unique_ptr<FieldObject>> fieldObjects_;
    std::string selectedObjectGUID_;

    FieldObject* selectedObject_ = nullptr;
    FieldObject_Start* startObject_ = nullptr;
    FieldObject_Goal* goalObject_ = nullptr;

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