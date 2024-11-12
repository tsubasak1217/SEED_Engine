#include "UFO.h"

UFO::UFO(){
    model_ = std::make_unique<Model>("UFO");
    model_->color_ = { 0.0f,1.0f,1.0f,1.0f };
    type_ = EnemyType::UFO;
    GetID();
}

UFO::~UFO(){}

void UFO::Update(){
    BaseEnemy::Update();
}
