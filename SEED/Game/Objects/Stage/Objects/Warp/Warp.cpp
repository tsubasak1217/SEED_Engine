#include "Warp.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/ClockManager/ClockManager.h>

//============================================================================
//	Warp classMethods
//============================================================================

void Warp::Initialize(const std::string& filename) {

    // スプライトの初期化
    sprite_ = Sprite(filename);
    sprite_.anchorPoint = Vector2(0.5f);
}

void Warp::Update() {

    // オブジェクトの更新を行う
    switch (commonState_) {
    case StageObjectCommonState::None:

        sprite_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
        break;
    case StageObjectCommonState::Hologram:

        sprite_.color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
        break;
    }

    // 常に行う更新処理
    UpdateAlways();
}

void Warp::UpdateAlways() {

    // 回転
    sprite_.rotate += std::numbers::pi_v<float> / 16.0f;
}

void Warp::Draw() {

    // スプライトの描画
    sprite_.Draw();
}