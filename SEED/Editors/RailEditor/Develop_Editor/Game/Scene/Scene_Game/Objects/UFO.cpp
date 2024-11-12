#include "UFO.h"

UFO::UFO(){
    model_ = std::make_unique<Model>("UFO");
    type_ = EnemyType::UFO;
    GetID();
}

UFO::~UFO(){}

void UFO::Update(){
    if(isActive_){

    }
    BaseEnemy::Update();
}
