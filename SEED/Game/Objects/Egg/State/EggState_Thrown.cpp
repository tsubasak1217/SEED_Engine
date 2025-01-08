#include "EggState_Thrown.h"

// Others State
#include "EggState_Break.h"

/// Engine
//manager
#include "ClockManager.h"
// math
#include "MyMath.h"
#include "MatrixFunc.h"


EggState_Thrown::EggState_Thrown(BaseCharacter* _egg,const Vector3& _velocity)
    :ICharacterState(_egg),
    velocity_(_velocity){}

EggState_Thrown::~EggState_Thrown(){}

void EggState_Thrown::Initialize(BaseCharacter* character){
    ICharacterState::Initialize(character);
}

void EggState_Thrown::Update(){
    MoveThrow();

    ManageState();
}

void EggState_Thrown::Draw(){}

const float kGravity = -9.8f;
void EggState_Thrown::MoveThrow(){
    velocity_.y -= kGravity * ClockManager::DeltaTime();
    // 方向を再計算
    velocity_ = MyMath::Normalize(velocity_) * MyMath::Length(velocity_);

    // 移動
    pCharacter_->HandleMove(velocity_);
}

void EggState_Thrown::ManageState(){
    // 地面に ついたら(仮)
    if(pCharacter_->GetWorldTranslate().y <= 0.0f){
        pCharacter_->SetTranslateY(0.0f);
        pCharacter_->ChangeState(new EggState_Break());
    }
}
