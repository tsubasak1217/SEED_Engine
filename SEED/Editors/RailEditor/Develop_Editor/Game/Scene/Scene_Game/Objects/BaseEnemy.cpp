#include "BaseEnemy.h"

int32_t BaseEnemy::nextObstacleID_ = 0;

BaseEnemy::BaseEnemy(){
    model_ = std::make_unique<Model>("cube");
    GetID();
}

void BaseEnemy::Update(){
    model_->UpdateMatrix();
}

void BaseEnemy::Draw(){
    model_->Draw();
}

void BaseEnemy::GetID(){
    obstacleID_ = nextObstacleID_;
    nextObstacleID_++;
}
