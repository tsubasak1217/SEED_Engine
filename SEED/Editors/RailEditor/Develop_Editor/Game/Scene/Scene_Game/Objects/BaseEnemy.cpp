#include "BaseEnemy.h"
#include <algorithm>
#include "Easing.h"

int32_t BaseEnemy::nextObstacleID_ = 0;

BaseEnemy::BaseEnemy(){
    model_ = std::make_unique<Model>("cube");
    GetID();
}

void BaseEnemy::Update(){
    model_->UpdateMatrix();

    if(isActive_){
        lifetime_ -= ClockManager::DeltaTime();
        float t = std::clamp((5.0f - lifetime_), 0.0f, 1.0f);
        float scale = EaseOutQuint(t);
        model_->scale_ = { scale,scale,scale };
    
        if(lifetime_ <= 0.0f){
            isAlive_ = false;
        }
    
    } else{
        model_->scale_ = { 0.0f,0.0f,0.0f };
    }
}

void BaseEnemy::Draw(){
    model_->Draw();
}

void BaseEnemy::GetID(){
    obstacleID_ = nextObstacleID_;
    nextObstacleID_++;
}
