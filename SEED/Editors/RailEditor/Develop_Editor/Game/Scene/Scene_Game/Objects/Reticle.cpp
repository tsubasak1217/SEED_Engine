#include "Reticle.h"
#include "Environment.h"

// レティクルの初期化
Reticle::Reticle(){
    reticle_ = std::make_unique<Sprite>("reticle.png");
    reticle_->size = { 128.0f,128.0f };
    reticle_->anchorPoint = { 0.5f,0.5f };
}

Reticle::~Reticle(){}

// レティクルの更新
void Reticle::Update(){

    // レティクルの移動
    Vector2 input = InputManager::GetStickValue(PAD_STICK::LEFT);
    input.y *= -1.0f;// y軸反転
    reticle_->translate += input * speed_;

    // レティクルの半径
    Vector2 radius = {
        reticle_->size.x * reticle_->scale.x * 0.5f,
        reticle_->size.y * reticle_->scale.y * 0.5f
    };

    // 画面外に出ないように制限
    reticle_->translate.x = std::clamp(reticle_->translate.x, radius.x, kWindowSizeX - radius.x);
    reticle_->translate.y = std::clamp(reticle_->translate.y, radius.y, kWindowSizeY - radius.y);
}


// レティクルの描画
void Reticle::Draw(){
    reticle_->Draw();
}
