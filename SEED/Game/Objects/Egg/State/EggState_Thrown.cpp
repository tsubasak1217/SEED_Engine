#include "EggState_Thrown.h"

// Others State
#include "EggState_Break.h"

//object
#include "Egg/Egg.h"

/// Engine
//manager
#include "ClockManager.h"
//lib
#include "JsonManager/JsonCoordinator.h"
// math
#include <cmath>
#include "MyMath.h"
#include "MatrixFunc.h"
#include "MyFunc.h"

EggState_Thrown::EggState_Thrown(BaseCharacter* _egg,const Vector3& _directionXY,float _rotateY,float _speed)
    :directionXY_(_directionXY),
    speed_(_speed),
    rotateY_(_rotateY){
    Initialize("Thrown",_egg);
}

EggState_Thrown::~EggState_Thrown(){
    pCharacter_->SetIsJump(false);
}

void EggState_Thrown::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    // 投げられる前の座標を記録
    beforePos_ = pCharacter_->GetWorldTranslate();

    JsonCoordinator::RegisterItem("Egg","weight",weight_);

    JsonCoordinator::RegisterItem("Egg","throwTime",throwTime_);
    leftTime_ = throwTime_;

    pCharacter_->SetIsDrop(true);
    pCharacter_->SetIsJump(true);
}

void EggState_Thrown::Update(){
    MoveThrow();
}

void EggState_Thrown::Draw(){}

const float kGravity = 9.8f;
void EggState_Thrown::MoveThrow(){
    Egg* pEgg = dynamic_cast<Egg*>(pCharacter_);
    Vector3 velocity =  pEgg->GetVelocity();
    if(velocity.y == 0.0f){

        leftTime_ -= ClockManager::DeltaTime();

        // xy 平面の放物線の位置を計算
        Vector2 parabolicXY = MyFunc::CalculateParabolic(Vector2(directionXY_.x,directionXY_.y),speed_,1.0f - (leftTime_ / throwTime_),kGravity * weight_);

        // XY 平面を 3次元に変換
        Vector3 parabolick3D = Vector3(0.0f,parabolicXY.y,parabolicXY.x) * RotateYMatrix(rotateY_);

        // 移動
        pCharacter_->SetTranslate(beforePos_ + parabolick3D);
    } else{
        velocity.y -= kGravity * weight_ * ClockManager::DeltaTime();
        pEgg->SetVelocity(velocity);
        pCharacter_->SetTranslate(pCharacter_->GetLocalTranslate() + pEgg->GetVelocity() * ClockManager::DeltaTime());
    }
}

void EggState_Thrown::ManageState(){
    // 地面に ついたら(仮)
    if(!pCharacter_->GetIsDrop()){
        Egg* pEgg = dynamic_cast<Egg*>(pCharacter_);
        pEgg->SetVelocity({0.0f,0.0f,0.0f});
        pCharacter_->ChangeState(new EggState_Break(pCharacter_));
    }
}
