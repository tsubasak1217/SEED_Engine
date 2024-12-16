#include "Bullet.h"

Bullet::Bullet(){
    bullet_ = MakeEqualQuad2D(radius_, { 1.0f,0.0f,0.0f,1.0f });
    bullet_.translate = position_;
    colliders_.push_back(ColliderObject());
}

Bullet::~Bullet(){}

void Bullet::Update(){
    position_ += moveDirection_ * speed_;
    bullet_.translate = position_;
    lifeTime_ -= ClockManager::DeltaTime();

    for(auto& collider : colliders_){
        collider.Update(position_, radius_);
    }
}

void Bullet::Draw(){
    SEED::DrawQuad2D(bullet_);
}
