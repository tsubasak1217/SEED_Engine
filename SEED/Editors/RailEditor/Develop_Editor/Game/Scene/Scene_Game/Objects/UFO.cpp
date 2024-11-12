#include "UFO.h"
#include "MyFunc.h"

UFO::UFO(){
    model_ = std::make_unique<Model>("UFO");
    type_ = EnemyType::UFO;
    GetID();
}

UFO::~UFO(){}

void UFO::Update(){
    if(isActive_){
        if(waitTime_ >  0.0f){
            waitTime_ -= ClockManager::DeltaTime();
            
            if(waitTime_ > 0.35f){
                model_->translate_ += direction_ * speed_;
            }

        } else{
            direction_ = MyMath::Normalize({ MyFunc::Random(-100.0f,100.0f),MyFunc::Random(-100.0f,100.0f),MyFunc::Random(-100.0f,100.0f) });
            waitTime_ = 0.7f;
        }
    }
    BaseEnemy::Update();
}
