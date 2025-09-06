#include "Warp.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>

//============================================================================
//	Warp classMethods
//============================================================================

void Warp::Initialize() {

    // スプライトの初期化
    sprite_ = Sprite("Scene_Game/StageObject/dottedLine.png");
    sprite_.anchorPoint = Vector2(0.5f);

    // 初期化値
    currentState_ = State::None;
}

void Warp::SetWarpIndex(uint32_t warpIndex) {

    // ワープインデックスを設定
    warpIndex_ = warpIndex;
    // ワープインデックスで色を決定する
    switch (warpIndex) {
    case 0: {

        // 赤
        sprite_.color = MyMath::FloatColor(0xe62f8bff);
        break;
    }
    case 1: {

        // シアン
        sprite_.color = MyMath::FloatColor(0x00ffffff);
        break;
    }
    case 2: {

        // 黄色
        sprite_.color = MyMath::FloatColor(0xffdc00ff);
        break;
    }
    }
}

void Warp::SetNotification() {

    // ワープ可能状態の時のみ通知可能
    if (currentState_ != State::None) {
        return;
    }

    // 通知させる
    currentState_ = State::Notification;
}

void Warp::SetNone() {

    // 通常状態に戻す
    currentState_ = State::None;
}

void Warp::Update() {

    // 常に行う更新処理
    UpdateAlways();
}

void Warp::UpdateState() {

}

void Warp::UpdateAlways() {

    // 回転
    sprite_.rotate += std::numbers::pi_v<float> / 16.0f;
}

void Warp::Draw() {

    // スプライトの描画
    sprite_.Draw();
}

void Warp::Edit() {

    // 現在の状態を表示
    ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));
    ImGui::Text("ownType: %s:", EnumAdapter<StageObjectCommonState>::ToString(commonState_));
}