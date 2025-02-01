#pragma once

// engine

//local
#include "../FieldObject/FieldObject.h"
#include "../FieldObject/FieldObjectName.h"
#include "FieldObject/Goal/FieldObject_Goal.h"
#include "FieldObject/Start/FieldObject_Start.h"
#include "../Routine/RoutineManager.h"
#include "PlayerCorpse/Manager/PlayerCorpseManager.h"

#include "../Game/Manager/EnemyManager.h"
#include "../Game/Editor/EnemyEditor.h" 

// FieldObject
#include "FieldObject/Door/FieldObject_Door.h"
#include "FieldObject/GrassSoil/FieldObject_GrassSoil.h"
#include "FieldObject/Soil/FieldObject_Soil.h"
#include "FieldObject/Star/FieldObject_Star.h"
#include "FieldObject/Switch/FieldObject_Switch.h"
#include "FieldObject/ViewPoint/FieldObject_ViewPoint.h"
#include "FieldObject/EventArea/FieldObject_EventArea.h"

// lib
#include "../lib/patterns/ISubject.h"
#include <vector>
#include <memory>

class Player;

class Stage{
public:
    Stage(ISubject& subject,uint32_t stageNo);

    // 卵の取得数など,ステータスの初期化(ステージを遷移したときなどに呼ぶ)
    void InitializeStatus();
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
    void SetSelectedObject(FieldObject * obj){ selectedObject_ = obj; }
    // 指定されたGUIDのオブジェクトを取得
    FieldObject* GetFieldObjectByGUID(const std::string & guid) const{
        for(const auto& objPtr : fieldObjects_){
            if(objPtr->GetGUID() == guid){
                return objPtr.get();
            }
        }
        return nullptr;
    }
    // 選択オブジェクトのGUIDを設定
    void SetSelectedObjectGUID(const std::string & guid){ selectedObjectGUID_ = guid; }
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
    template <typename T>
    std::vector<T*> GetObjectsOfType();
    void SetPlayer(Player * pPlayer){
        pPlayer_ = pPlayer;
        enemyManager_->SetPlayer(pPlayer);
    }
    EnemyManager* GetEnemyManager(){ return enemyManager_.get(); }
    // RoutineManagerのゲッターを追加
    RoutineManager& GetRoutineManager(){ return routineManager_; }
    // PlayerCorpseManagerのゲッターを追加
    PlayerCorpseManager* GetPlayerCorpseManager(){ return playerCorpseManager_.get(); }

    /// <summary>
    /// 現在の取得した星の数
    /// </summary>
    /// <returns></returns>
    int32_t GetCurrentStarCount()const{ return currentStarCount_; }

private:
    int32_t stageNo_ = -1;
    uint32_t difficulty_ = 0;
    std::vector<std::unique_ptr<FieldObject>> fieldObjects_;
    std::string selectedObjectGUID_;

    // 現在の取得した星の数
    uint32_t currentStarCount_ = 0;
    // ハイスコア
    // uint32_t heightStarCount_ = 0; <- StageManager::getStarCounts_ があったので削除

    FieldObject* selectedObject_ = nullptr;
    //特殊処理 をするため ポインターを個別で保持
    FieldObject_Start* startObject_ = nullptr;
    FieldObject_Goal* goalObject_ = nullptr;
    std::vector<FieldObject_Star*> starObjects_;

    ISubject& subject_;

    //playerのポインタ
    Player* pPlayer_ = nullptr;
    //playerの死体
    std::unique_ptr<PlayerCorpseManager> playerCorpseManager_;

    //enemy
    std::unique_ptr<EnemyManager> enemyManager_;

    //routine
    RoutineManager routineManager_;
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