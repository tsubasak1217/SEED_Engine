#include "Bullet.h"

// 弾の初期化
Bullet::Bullet(){
    bullet_ = std::make_unique<Model>("sphere");
}

// 弾の初期化
Bullet::Bullet(const Vector3& position, const Vector3& direction){
    bullet_ = std::make_unique<Model>("plane");
    bullet_->textureGH_ = TextureManager::LoadTexture("star.png");
    bullet_->lightingType_ = LIGHTINGTYPE_NONE;
    bullet_->translate_ = position;
    direction_ = direction;
    bullet_->UpdateMatrix();
}

Bullet::~Bullet(){}

// 弾の更新
void Bullet::Update(){

    float t = std::clamp(lifetime_, 0.0f, 1.0f);
    
    // 弾の移動
    bullet_->translate_ += direction_ * speed_ * ClockManager::DeltaTime();
    bullet_->rotate_.y += 6.28f * ClockManager::DeltaTime();
    bullet_->scale_ = { 0.5f * t,0.5f * t,0.5f * t };
    bullet_->UpdateMatrix();

    // 弾の寿命の更新
    lifetime_ -= ClockManager::DeltaTime();
}

// 弾の描画
void Bullet::Draw(){
    bullet_->Draw();
}
