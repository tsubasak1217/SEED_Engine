#include "Ghost.h"

Ghost::Ghost(){
    model_ = std::make_unique<Model>("Ghost");
    type_ = EnemyType::Ghost;
    GetID();
}

Ghost::~Ghost(){}

void Ghost::Update(){
    if(isActive_){

    }
    BaseEnemy::Update();
}
