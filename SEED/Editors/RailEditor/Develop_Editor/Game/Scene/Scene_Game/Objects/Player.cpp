#include "Player.h"
#include "MatrixFunc.h"
#include "Environment.h"

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
    forward_ = Vector3(0.0f, 0.0f, 1.0f) * RotateMatrix(player_->rotate_);// 前方ベクトルの計算
    player_->UpdateMatrix();

    ///////////////////////////////////////////////
    // レティクルの更新
    ///////////////////////////////////////////////
    if(!pRailCamera_->GetIsDebugCameraActive()){
        if(pRailCamera_->GetT() > 0.0f){
            reticle_->Update();
        }
    }


    ///////////////////////////////////////////////
    // 弾の更新
    ///////////////////////////////////////////////

    Shoot();

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


    if(!pRailCamera_->GetIsDebugCameraActive()){
        // レティクルの描画
        if(pRailCamera_->GetT() > 0.0f){
            reticle_->Draw();
        }
    } else{
        // プレイヤーの描画
        player_->Draw();
    }

    // 弾の描画
    for(auto& bullet : bullets_){
        bullet->Draw();
    }
}

void Player::Shoot(){


    ///////////////////////////////////////////////
    // 発射ベクトルの計算
    ///////////////////////////////////////////////
    Matrix4x4 inverseVPV = InverseMatrix(pRailCamera_->vpVp_);
    Vector3 nearPos = { kWindowCenter.x,kWindowSizeY,0.0f };
    Vector3 farPos = { reticle_->GetPosition().x,reticle_->GetPosition().y, SEED::GetCamera()->zfar_ };

    // ワールド座標へ
    nearPos = nearPos * inverseVPV;
    farPos = farPos * inverseVPV;

    // 正規化ベクトルにする
    beamVec_ = MyMath::Normalize(farPos - nearPos);


    ///////////////////////////////////////////////
    // 弾の追加
    ///////////////////////////////////////////////
    if(!pRailCamera_->GetIsDebugCameraActive()){
        if(InputManager::IsPressPadButton(PAD_BUTTON::A)){

            if(shotGage_ > 0.0f){
                float beamWidth = 1.0f;
                Vector3 v[4] = {
                Vector3(reticle_->GetPosition().x - beamWidth,reticle_->GetPosition().y,SEED::GetCamera()->zfar_) * inverseVPV,
                Vector3(reticle_->GetPosition().x + beamWidth,reticle_->GetPosition().y,SEED::GetCamera()->zfar_) * inverseVPV,
                Vector3(kWindowCenter.x * 0.5f,kWindowSizeY,SEED::GetCamera()->znear_) * inverseVPV,
                Vector3(kWindowCenter.x * 1.5f,kWindowSizeY,SEED::GetCamera()->znear_) * inverseVPV
                };

                Matrix4x4 translateMat = TranslateMatrix({ 0.0f,ClockManager::TotalTime() * 2.0f,0.0f });

                SEED::DrawQuad(
                    v[0], v[1], v[2], v[3],
                    TextureManager::LoadTexture("beam.png"),
                    { 1.0f,1.0f,1.0f,1.0f },
                    LIGHTINGTYPE_NONE,
                    ScaleMatrix({ 1.0f,SEED::GetCamera()->zfar_,1.0f }) * translateMat
                );

                isBeam_ = true;
                shotGage_ = std::clamp(shotGage_ - 0.01f, 0.0f, 1.0f);
            }

        } else{
            isBeam_ = false;
            shotGage_ = std::clamp(shotGage_ + 0.01f, 0.0f, 1.0f);
        }
    }
}
