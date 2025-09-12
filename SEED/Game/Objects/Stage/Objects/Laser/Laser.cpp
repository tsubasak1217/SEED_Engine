#include "Laser.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Laser/Methods/LaserHelper.h>

//============================================================================
//	Laser classMethods
//============================================================================

static uint64_t gLaserFamilyIdSequence = 1;

void Laser::Initialize() {

    // スプライトの初期化
    sprite_ = Sprite("Scene_Game/StageObject/laser.png");
    sprite_.anchorPoint = Vector2(0.5f, 1.0f);
    sprite_.layer = 10;
    sprite_.blendMode = BlendMode::ADD;

    // 初期化値
    warpParam_.isHit = false;

    // 初期レーザー作成時に新しい系統IDを設定
    if (familyId_ == 0) {

        familyId_ = gLaserFamilyIdSequence++;
    }
}

void Laser::SetDirection(DIRECTION4 direction) {

    // 向きの設定
    direction_ = direction;
    // 向きで回転を設定
    sprite_.transform.rotate = LaserHelper::GetRotateFromDirection(direction);
    owner_->SetWorldRotate(sprite_.transform.rotate);
}

void Laser::ReExtend() {

    // 再起動する
    currentState_ = State::Extend;
}

void Laser::StopExtend() {

    // 停止させる
    currentState_ = State::Stop;
    // ここで一度だけ衝突を更新
    owner_->SetWorldScale(Vector2(1.0f, sprite_.size.y / initSizeY_.value()));
}

void Laser::SetHitWarpParam(const WarpLaserParam& param) {

    // 値を設定
    warpParam_.isHit = param.isHit;
    warpParam_.warpIndex = param.warpIndex;
    warpParam_.warpCommonState = param.warpCommonState;
}

bool Laser::IsStopAll() const {

    return currentState_ == State::Stop && sprite_.size.y <= initSizeY_.value() / 2.0f;
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

    if (isStop_) {
        return;
    }

    sizeExtendSpeed_ = 18.0f;

    // オブジェクトに衝突するまで伸びつ続ける
    LaserHelper::UpdateLaserSprite(sprite_, sizeExtendSpeed_);
    // スケーリング値の更新
    owner_->SetWorldScale(Vector2(1.0f, sprite_.size.y / initSizeY_.value()));
}

void Laser::UpdateStop() {
}

void Laser::Draw() {

    // スプライトの描画
    sprite_.Draw();
}