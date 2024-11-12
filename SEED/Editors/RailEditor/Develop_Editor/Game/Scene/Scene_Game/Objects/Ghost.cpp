#include "Ghost.h"

Ghost::Ghost(){
    model_ = std::make_unique<Model>("Ghost");
    model_->color_ = { 0.0f,0.0f,1.0f,1.0f };
    type_ = EnemyType::Ghost;
    GetID();
}

Ghost::~Ghost(){}

void Ghost::Update(){
    BaseEnemy::Update();
}
