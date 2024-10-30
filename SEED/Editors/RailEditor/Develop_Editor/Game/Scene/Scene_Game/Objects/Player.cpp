#include "Player.h"
#include "MatrixFunc.h"

Player::Player(){}

Player::Player(const Vector3& position){
    // プレイヤーの初期化
    player_ = std::make_unique<Model>("suzanne");
    player_->translate_ = position;
    player_->UpdateMatrix();

    // レティクルの初期化
    reticle_ = std::make_unique<Reticle>();
}

Player::~Player(){}

// 更新
void Player::Update(){


    ///////////////////////////////////////////////
    // トランスフォームの更新
    ///////////////////////////////////////////////
    player_->translate_ = pRailCamera_->transform_.translate_;
    player_->rotate_ = pRailCamera_->transform_.rotate_;
    player_->UpdateMatrix();

    ///////////////////////////////////////////////
    // レティクルの更新
    ///////////////////////////////////////////////
    if(!pRailCamera_->GetIsDebugCameraActive()){
        reticle_->Update();
    }

    ///////////////////////////////////////////////
    // 発射ベクトルの計算
    ///////////////////////////////////////////////
    Matrix4x4 inverseVPV = InverseMatrix(pRailCamera_->vpVp_);
    Vector3 nearPos = {reticle_->GetPosition().x,reticle_->GetPosition().y,0.0f};
    Vector3 farPos = { reticle_->GetPosition().x,reticle_->GetPosition().y,1.0f };

    // ワールド座標へ
    nearPos = nearPos * inverseVPV;
    farPos = farPos * inverseVPV;

    // 正規化ベクトルにする
    Vector3 shootVector = MyMath::Normalize(farPos - nearPos);

    // 前方ベクトルの計算
    forward_ = Vector3(0.0f, 0.0f, 1.0f) * RotateMatrix(player_->rotate_);

    ///////////////////////////////////////////////
    // 弾の追加
    ///////////////////////////////////////////////
    if(!pRailCamera_->GetIsDebugCameraActive()){
        if(InputManager::IsTriggerPadButton(PAD_BUTTON::B)){
            bullets_.emplace_back(
                std::make_unique<Bullet>(player_->translate_, shootVector)
            );
        }
    }

    ///////////////////////////////////////////////
    // 弾の更新
    ///////////////////////////////////////////////
    for(auto& bullet : bullets_){
        bullet->Update();
    }

    ///////////////////////////////////////////////
    // 弾の削除
    ///////////////////////////////////////////////
    bullets_.remove_if([](const std::unique_ptr<Bullet>& bullet){
        return !bullet->GetIsAlive();
    });

}

// 描画
void Player::Draw(){

    // プレイヤーの描画
    player_->Draw();
    
    // レティクルの描画
    if(!pRailCamera_->GetIsDebugCameraActive()){
        reticle_->Draw();
    }

    // 弾の描画
    for(auto& bullet : bullets_){
        bullet->Draw();
    }
}
