#include "Goal.h"
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>
#include <Game/Components/StageObjectComponent.h>

//============================================================================
//	Goal classMethods
//============================================================================

void Goal::Initialize(){

    static bool isFirstInitialize = true;
    if(isFirstInitialize){
        imageMap_["crown"] = TextureManager::LoadTexture("Scene_Game/StageObject/crown.png");
        imageMap_["crown_outline"] = TextureManager::LoadTexture("Scene_Game/StageObject/crown_outline.png");
        isFirstInitialize = false;
    }

    // スプライトの初期化
    sprite_ = Sprite(imageMap_["crown"]);
    sprite_.anchorPoint = Vector2(0.5f);

}

void Goal::Update(){

    // オブジェクトの更新を行う
    switch(commonState_){

    case StageObjectCommonState::None:
        sprite_.color = normalColor_;
        sprite_.GH = imageMap_["crown_outline"];
        break;

    case StageObjectCommonState::Hologram:
        sprite_.color = hologramColor_;
        sprite_.GH = imageMap_["crown_outline"];
        break;
    }

    // 王冠取得時の動き
    CrownMotion();
}

void Goal::Draw(){

    // スプライトの描画
    sprite_.Draw();
}


//////////////////////////////////////////////////////////////////////////////////
// 衝突時の処理
//////////////////////////////////////////////////////////////////////////////////
void Goal::OnCollisionEnter([[maybe_unused]] GameObject2D* other){

    // もしプレイヤーに触れられたら
    if(other->GetObjectType() == ObjectType::Player){
        // プレイヤーのゴールタッチ時間を増加させる
        pPlayer_ = other->GetComponent<StageObjectComponent>()->GetStageObject<Player>();
        startPos_ = sprite_.transform.translate;
    }
}

void Goal::OnCollisionStay([[maybe_unused]] GameObject2D* other){

}

void Goal::OnCollisionExit([[maybe_unused]] GameObject2D* other){
    if(other->GetObjectType() == ObjectType::Player){
        if(pPlayer_){
            // ゴールから離れた場合,タイマーをリセット
            pPlayer_->ResetGoalTouchTime();
            pPlayer_ = nullptr;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////
// 王冠の動き
///////////////////////////////////////////////////////////////////////////////////
void Goal::CrownMotion(){

    if(pPlayer_){

        // プレイヤーの上をターゲットにする
        targetPos_ = pPlayer_->GetOwner()->GetWorldTranslate() + defaultTileSize_ * 0.6f * Vector2(0.0f, -1.0f);

        // スプライン曲線で補間
        std::vector<Vector2> points = {
            startPos_,
            startPos_ + ((targetPos_ - startPos_) / 3.0f) * 1 + Vector2(0.0f, -100.0f),
            startPos_ + ((targetPos_ - startPos_) / 3.0f) * 2 + Vector2(0.0f, -100.0f),
            targetPos_
        };

        // playerから時間を取得
        float t = pPlayer_->GetGoalT();
        float easeT = EaseInOutQuad(t);

        // 座標を計算
        sprite_.transform.translate = MyMath::CatmullRomPosition(points, easeT);
        savedPos_ = sprite_.transform.translate;
        sprite_.layer = 12;// プレイヤー上に描画

    } else{

        if(MyMath::Length(targetPos_ - savedPos_) > 1.0f){
            // プレイヤーに触れられていない場合,徐々に元の位置に戻す
            sprite_.transform.translate = savedPos_ + (owner_->GetWorldTranslate() - savedPos_) * 0.1f * ClockManager::TimeRate();
            savedPos_ = sprite_.transform.translate;
            motionTimer_ = 0.0f;
        } else{
            // 上下に揺らす
            float waveRadius = 5.0f;
            sprite_.transform.translate.y = sprite_.transform.translate.y + std::sinf(motionTimer_ * 3.14f * 0.7f) * waveRadius;
        }

        sprite_.layer = 0;
    }

    // 時間の更新
    motionTimer_ += ClockManager::DeltaTime();
}