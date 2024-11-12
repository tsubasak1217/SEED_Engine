#include "Ghost.h"
#include "MyFunc.h"

Ghost::Ghost(){
    model_ = std::make_unique<Model>("Ghost");
    type_ = EnemyType::Ghost;
    direction_ = MyMath::Normalize({ MyFunc::Random(-100.0f,100.0f),MyFunc::Random(-100.0f,100.0f),MyFunc::Random(-100.0f,100.0f) });
    GetID();
}

Ghost::~Ghost(){}

void Ghost::Update(){
    if(isActive_){
        model_->translate_ += direction_ * speed_;
    }
    BaseEnemy::Update();
}
