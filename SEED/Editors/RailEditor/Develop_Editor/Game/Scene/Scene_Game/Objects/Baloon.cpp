#include "Baloon.h"

Baloon::Baloon(){
    model_ = std::make_unique<Model>("Baloon");
    model_->color_ = { 1.0f,0.0f,0.0f,1.0f };
    type_ = EnemyType::Baloon;
    GetID();
}

Baloon::~Baloon(){}

void Baloon::Update(){
    BaseEnemy::Update();
}
