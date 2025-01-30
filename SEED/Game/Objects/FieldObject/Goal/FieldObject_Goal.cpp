#include "FieldObject_Goal.h"
#include "StageManager.h"
#include "SEED.h"

////////////////////////////////////////////////////////////////////////////////////////
//  コンストラクタ
////////////////////////////////////////////////////////////////////////////////////////
FieldObject_Goal::FieldObject_Goal(){
    className_ = "FieldObject_Goal";
    name_ = "goal";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    InitColliders(ObjectType::GoalField);
    // UIの初期化
    goalUI_ = std::make_unique<Sprite>("GameUI/A.png");
    goalUI_->anchorPoint = Vector2(0.5f, 0.5f);
    // 全般の初期化
    FieldObject::Initialize();
}

FieldObject_Goal::FieldObject_Goal(const std::string& modelName)
    : FieldObject(modelName){
    // クラス名の初期化
    className_ = "FieldObject_Goal";
    name_ = "goal";
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    InitColliders(ObjectType::GoalField);
    // UIの初期化
    goalUI_ = std::make_unique<Sprite>("GameUI/A.png");
    goalUI_->anchorPoint = Vector2(0.5f, 0.5f);
    // 全般の初期化
    FieldObject::Initialize();
}

////////////////////////////////////////////////////////////////////////////////////////
//  初期化関数
////////////////////////////////////////////////////////////////////////////////////////
void FieldObject_Goal::Initialize(){
    isGoal_ = false;
}

////////////////////////////////////////////////////////////////////////////////////////
//  更新関数
////////////////////////////////////////////////////////////////////////////////////////
void FieldObject_Goal::Update(){
    isTouchedPlayer_ = false;
    FieldObject::Update();
    goalUI_->translate = SEED::GetCamera()->ToScreenPosition(GetWorldTranslate() + Vector3(0.0f,6.0f,0.0f));
}

////////////////////////////////////////////////////////////////////////////////////////
//  描画関数
////////////////////////////////////////////////////////////////////////////////////////
void FieldObject_Goal::Draw(){
    // ゴールに触れたら現れるUIを表示
    goalUI_->color.w = std::clamp(playerTouchTime_ / 1.0f,0.0f,1.0f);
    goalUI_->Draw();

    // モデルの描画
    FieldObject::Draw();
}

////////////////////////////////////////////////////////////////////////////////////////
//  フレーム終了処理
////////////////////////////////////////////////////////////////////////////////////////
void FieldObject_Goal::EndFrame(){
    FieldObject::EndFrame();
    if(isTouchedPlayer_){
        playerTouchTime_ += ClockManager::DeltaTime();
    } else{
        playerTouchTime_ -= ClockManager::DeltaTime();
    }

    playerTouchTime_ = std::clamp(playerTouchTime_, 0.0f, 1.0f);
}


////////////////////////////////////////////////////////////////////////////////////////
//  衝突処理
////////////////////////////////////////////////////////////////////////////////////////
void FieldObject_Goal::OnCollision(const BaseObject* other,ObjectType objectType){
    other;
    if(objectType == ObjectType::Player){
        // ゴールしたことを通知
        isGoal_ = true;

        // プレイヤーが触れたことを記録
        isTouchedPlayer_ = true;
    }
}
