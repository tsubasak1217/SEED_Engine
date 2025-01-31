#include "EggState_Thrown.h"

// Others State
#include "EggState_Break.h"

//object
#include "Egg/Egg.h"

/// Engine
//manager
#include "ClockManager.h"
//lib
#include "../adapter/json/JsonCoordinator.h"
// math
#include <cmath>
#include "MyMath.h"
#include "MatrixFunc.h"
#include "MyFunc.h"

EggState_Thrown::EggState_Thrown(BaseCharacter* _egg,const Vector3& p1, const Vector3& p2, const Vector3& p3){
    Initialize("Thrown",_egg);
    controlPoints_[0] = p1;
    controlPoints_[1] = p2;
    controlPoints_[2] = p3;
}

EggState_Thrown::~EggState_Thrown(){}

void EggState_Thrown::Initialize(const std::string& stateName,BaseCharacter* character){
    ICharacterState::Initialize(stateName,character);

    // 投げられる前の座標を記録
    beforePos_ = pCharacter_->GetWorldTranslate();

    JsonCoordinator::RegisterItem("Egg","weight",weight_);

    JsonCoordinator::RegisterItem("Egg","throwTime",throwTime_);
    leftTime_ = throwTime_;

    pEgg_ = dynamic_cast<Egg*>(pCharacter_);
    pEgg_->SetIsThrown(true);
    // 当たり判定を取るように
    pEgg_->SetCollidable(true);
}

void EggState_Thrown::Update(){
    MoveThrow();
}

void EggState_Thrown::Draw(){}

const float kGravity = 9.8f;
void EggState_Thrown::MoveThrow(){
    Vector3 velocity =  pEgg_->GetVelocity();
    if(velocity.y == 0.0f){

        leftTime_ -= ClockManager::DeltaTime();
        float t = throwTime_ - leftTime_;

        // 位置の更新
        pCharacter_->SetTranslate(MyMath::Bezier(controlPoints_[0], controlPoints_[1], controlPoints_[2],t));

    } else{
        velocity.y -= kGravity * weight_ * ClockManager::DeltaTime();
        pEgg_->SetVelocity(velocity);
        pCharacter_->SetTranslate(pCharacter_->GetLocalTranslate() + pEgg_->GetVelocity() * ClockManager::DeltaTime());
    }
}

void EggState_Thrown::ManageState(){

    // 地面ではなく,y==0に落下したら削除
    if(pCharacter_->GetWorldTranslate().y <= 0.0f){
        pCharacter_->SetIsAlive(false);
        return;
    }
    // 地面に ついたら(仮)
    if(!pCharacter_->GetIsDrop()){
        pEgg_->SetVelocity({0.0f,0.0f,0.0f});
        pCharacter_->ChangeState(new EggState_Break(pCharacter_));
    }
}
