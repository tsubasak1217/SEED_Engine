// FieldObject_Switch.cpp
#include "FieldObject_Switch.h"
#include "FieldObject/Door/FieldObject_Door.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "Player/Player.h"

////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////////
FieldObject_Switch::FieldObject_Switch(){
    // 名前の初期化
    className_ = "FieldObject_Switch";
    name_ = "switch";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".gltf";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    model_->StartAnimation(1, false);
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Area);
    // 全般の初期化
    FieldObject::Initialize();

    //objectのClear
    associatedDoors_.clear();
    associatedMoveFloors_.clear();

    //ui
    weightUI_ = std::make_unique<SwitchWeightUI>(this);
    weightUI_->Initialize();
}

////////////////////////////////////////////////////////////////////
// 描画関数
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::Draw(){
    // 重さに応じてマテリアルを変更
    int leftrequiredWeight = std::clamp(int((float)requiredWeight_ - currentWeight_), 0, 100);
    // 重さに応じて色を変更
    switch(leftrequiredWeight){
    case 0:// 緑
        model_->color_ = { 1.0f,0.0f,0.0f,1.0f };
        break;
    case 1:// 黄
        break;
    case 2:// オレンジ
        break;
    default:// 赤
        break;
    }
    FieldObject::Draw();

    if(drawingUI_){
        weightUI_->Draw();
    }

}

////////////////////////////////////////////////////////////////////
// フレーム開始時処理
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::BeginFrame(){
    if (isColliding_ && !isActivated_){
        // 乗っている状態で、まだ起動状態でなければ
        isActivated_ = true;

        // SE再生
        AudioManager::PlayAudio("SE/switch.wav",false,0.7f);

        Notify("SwitchActivated", this);
    } else if (!isColliding_ && isActivated_){
        // 何も乗っていない状態で、現在起動状態であれば
        isActivated_ = false;
        Notify("SwitchDeactivated", this);
    }
    // 基底クラスの BeginFrame() を呼ぶ（重さをリセット＆判定などを行う）
    FieldObject_Activator::BeginFrame();
}

////////////////////////////////////////////////////////////////////
// フレーム終了時処理
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::EndFrame(){
    FieldObject::EndFrame();
    // 必要重量を満たしていればスイッチをオンにする
    if((int)currentWeight_ >= requiredWeight_){
        isColliding_ = true;
        model_->StartAnimation(1, false);
    } else{
        isColliding_ = false;
        model_->StartAnimation(0, false);
    }

    drawingUI_ = false;
    const float distanceToDrawUI = 10.f;
    if(MyMath::Length(player_->GetWorldTranslate() - GetWorldTranslate()) < distanceToDrawUI){
        drawingUI_ = true;
        weightUI_->Update();
    }
}

////////////////////////////////////////////////////////////////////
// 衝突時処理
////////////////////////////////////////////////////////////////////
void FieldObject_Switch::OnCollision(BaseObject* other,ObjectType objectType){
    // 重さを加算
    if(objectType == ObjectType::Player or objectType == ObjectType::Egg or objectType == ObjectType::PlayerCorpse
        or objectType == ObjectType::Enemy){
        currentWeight_ += other->GetSwitchPushWeight();
    }

}