#include "Egg.h"

// State
#include "Egg/State/EggState_Follow.h"

//Engine
#include "ClockManager.h"

#include "MyMath.h"

// Object
#include "Model.h"
#include "Player/Player.h"

// Manager
#include "../adapter/json/JsonCoordinator.h"
#include "Manager/EggManager.h"
#include "StageManager.h"

Egg::Egg(BaseObject* _player)
    :BaseCharacter(),
    player_(_player){
    className_ = "Egg";
    isApplyGravity_ = false;
}

Egg::~Egg(){}

void Egg::Initialize(){
    // model 読み込み
    model_ = std::make_unique<Model>("egg.obj");
    model_->isParentScale_ = false;

    // 卵がないなら Player に 追従する, あるなら 最後尾の卵に追従する
    if(eggManager_->GetIsEmpty()){
        currentState_ = std::make_unique<EggState_Follow>(this,this->player_);
    } else{
        currentState_ = std::make_unique<EggState_Follow>(this,eggManager_->GetBackEgg().get());
    }

    JsonCoordinator::RegisterItem("Egg","weight",weight_);

    // コライダーエディターの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_,this);
    LoadColliders(ObjectType::Egg);
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(ObjectType::Egg);
        collider->AddSkipPushBackType(ObjectType::Editor);
        collider->AddSkipPushBackType(ObjectType::Enemy);
        collider->AddSkipPushBackType(ObjectType::Player);
    }
    SetCollidable(false);
    isApplyGravity_ = false;

    // shadow
    shadow_ = std::make_unique<Shadow>(this);
    shadow_->Initialize();
}

void Egg::Update(){
    BaseCharacter::Update();
    EditCollider();

    //! TODO : Stage の 取得方法
    shadow_->Update(StageManager::GetCurrentStage());

}

void Egg::Draw(){

    shadow_->Draw();

    BaseCharacter::Draw();
}

void Egg::OnCollision([[maybe_unused]] const BaseObject* other,ObjectType objectType){
    BaseObject::OnCollision(other,objectType);

    if(objectType == ObjectType::Field){

        if(!isThrown_){
            return;
        }

        // 衝突した場所の真下に落ちる
        // XZ を固定
        SetTranslateX(prePos_.x);
        SetTranslateZ(prePos_.z);

        velocity_.y = -MyMath::Length(this->GetWorldTranslate() - prePos_);
    }

    // 移動床に触れている状態
    if((int32_t)objectType & (int32_t)ObjectType::Move){
        if(!IsJumpable()){
            // 親子付けを行い移動床基準のトランスフォームに変換
            SetParent(other);

            Vector3 preTranslate = GetWorldTranslate();
            Matrix4x4 invParentMat = InverseMatrix(GetParent()->GetWorldMat());
            Vector3 localTranslate = preTranslate * invParentMat;
            localTranslate *= ExtractScale(GetParent()->GetWorldMat());
            SetTranslate(localTranslate);
            UpdateMatrix();
        }
    }
}

void Egg::SpawnPlayer(){
    //Player を spawn させる
    Player* player = dynamic_cast<Player*>(player_);
    if(player_){
        player->Spawn(this);
    }
}
