#include "BaseCharacter.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include "ICharacterState.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
//////////////////////////////////////////////////////////////////////////
BaseCharacter::BaseCharacter() : BaseObject(){
    name_ = "unnamed";
    Initialize();
}

BaseCharacter::~BaseCharacter(){}

void BaseCharacter::Initialize(){
    // モデルの初期化
    model_ = std::make_unique<Model>("Assets/man.gltf");
    model_->UpdateMatrix();
    model_->isRotateWithQuaternion_ = false;
}


//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::Update(){

    // 前フレームの座標の保存
    prePos_ = GetWorldTranslate();

    // 状態に応じた更新処理
    if(currentState_){
        currentState_->Update();
    }

    BaseObject::Update();
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::Draw(){

    // 状態に応じた描画処理
    if(currentState_){
        currentState_->Draw();
    }

    // 基本モデルの描画
    BaseObject::Draw();
}


//////////////////////////////////////////////////////////////////////////
// ステート関連
//////////////////////////////////////////////////////////////////////////
void BaseCharacter::ChangeState(ICharacterState* nextState){
    currentState_.reset(nextState);
}

void BaseCharacter::HandleMove(const Vector3& acceleration){
    // 移動
    model_->translate_ += acceleration;
}

void BaseCharacter::HandleRotate(const Vector3& rotate){
    // 回転
    model_->rotate_ = rotate;
    model_->UpdateMatrix();
}

//////////////////////////////////////////////////////////////////////////
// コライダー関連
//////////////////////////////////////////////////////////////////////////

void BaseCharacter::AddCollider(Collider* collider){
    colliders_.push_back(collider);
}

void BaseCharacter::ResetCollider(){
    colliders_.clear();
}

void BaseCharacter::HandOverColliders(){

    // キャラクターの基本コライダーを渡す
    for(auto& collider : colliders_){
        CollisionManager::AddCollider(collider);
    }

    // state固有のcolliderも渡す
    if(currentState_){
        currentState_->HandOverColliders();
    }
}
