#include "Laser.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Laser/Methods/LaserHelper.h>

//============================================================================
//	Laser classMethods
//============================================================================

void Laser::Initialize() {

    // スプライトの初期化
    sprite_ = Sprite("Scene_Game/StageObject/laserForward.png");
    sprite_.anchorPoint = Vector2(0.5f, 1.0f);
}

void Laser::SetDirection(DIRECTION4 direction) {

    // 向きの設定
    direction_ = direction;
    // 向きで回転を設定
    sprite_.rotate = LaserHelper::GetRotateFromDirection(direction);
    owner_->SetWorldRotate(sprite_.rotate);
}

void Laser::ReExtend() {

    // 再起動する
    currentState_ = State::Extend;
}

void Laser::StopExtend() {

    // 停止させる
    currentState_ = State::Stop;
}

void Laser::Update() {

    // 状態に応じて更新処理
    switch (currentState_) {
    case ILaserObject::State::Extend: {

        // 伸びている時の処理
        UpdateExtend();
        break;
    }
    case ILaserObject::State::Stop: {

        // 止まっているときの更新処理
        UpdateStop();
        break;
    }
    }
}

void Laser::UpdateExtend() {

    sizeExtendSpeed_ = 16.0f;

    // オブジェクトに衝突するまで伸びつ続ける
    LaserHelper::UpdateLaserSprite(sprite_, sizeExtendSpeed_);
    // スケーリング値の更新
    owner_->SetWorldScale(Vector2(1.0f, sprite_.size.y / initSizeY_));
}

void Laser::UpdateStop() {


}

void Laser::Draw() {

    // スプライトの描画
    sprite_.Draw();
}