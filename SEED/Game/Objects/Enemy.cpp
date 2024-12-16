#include "Enemy.h"
#include "MyFunc.h"

Enemy::Enemy(){
    enemy_ = MakeEqualQuad2D(radius_, { 0.0f,1.0f,0.0f,1.0f });
    enemy_.translate = MyFunc::Random({ 0.0f,0.0f }, { 1280.0f,360.0f });
    radius_ = 30.0f;
    colliders_.push_back(ColliderObject());
}

Enemy::~Enemy(){}

void Enemy::Update(){
    for(auto& collider : colliders_){
        collider.Update(enemy_.translate, radius_);
    }
}

void Enemy::Draw(){
    SEED::DrawQuad2D(enemy_);
}
