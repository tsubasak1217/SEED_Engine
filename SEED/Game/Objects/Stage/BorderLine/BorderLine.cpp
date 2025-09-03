#include "BorderLine.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Source/SEED.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	BorderLine classMethods
//============================================================================

void BorderLine::Initialize() {

    // スプライトを初期化
    sprite_ = Sprite("DefaultAssets/checkerBoard.png");
    sprite_.color.w = 0.8f;

    // 初期状態を設定
    currentState_ = State::Disable;
}

void BorderLine::SetActivate(const Vector2& translate, float sizeY) {

    // アクティブ状態を設定
    currentState_ = State::Active;
    sprite_.translate = translate;
    // ウィンドウの一番上にサイズを合わせる
    sprite_.size.y = sizeY;
}

void BorderLine::SetDeactivate() {

    // 非アクティブにする
    currentState_ = State::Disable;
}

bool BorderLine::CheckPlayerToDistance(float playerX) const {

    // 状態がアクティブじゃないときは判定しない
    if (currentState_ == State::Disable) {
        return false;
    }

    // プレイヤーとの距離が近ければtrueを返す
    float distance = playerX - sprite_.translate.x;
    return std::fabs(distance) < playerToDistance_;
}

bool BorderLine::CanTransitionDisable(float playerX) const {

    return IsActive() && CheckPlayerToDistance(playerX);
}

void BorderLine::Update() {


}

void BorderLine::Draw() {

    // アクティブ状態じゃなければ表示しない
    if (currentState_ == State::Disable) {
        return;
    }

    // 描画
    sprite_.Draw();
}

void BorderLine::Edit(const Sprite& playerSprite) {

    // 現在の状態を表示
    ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));

    ImGui::Separator();

    ImGui::DragFloat("playerToDistance", &playerToDistance_, 0.1f);
    ImGui::DragFloat("spriteSizeX", &sprite_.size.x, 0.1f);
    ImGui::DragFloat2("spriteAnchor", &sprite_.anchorPoint.x, 0.05f);

    // プレイヤーと境界線
    Vector2 left(sprite_.translate.x - playerToDistance_, sprite_.translate.y);
    Vector2 right(sprite_.translate.x + playerToDistance_, sprite_.translate.y);
    if (CheckPlayerToDistance(playerSprite.translate.x)) {

        SEED::DrawLine2D(left, right, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    } else {

        SEED::DrawLine2D(left, right, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    }
}

void BorderLine::FromJson(const nlohmann::json& data) {

    sprite_.size.x = data["sprite_.size.x"];
    playerToDistance_ = data.value("playerToDistance_", 16.0f);
    from_json(data.value("sprite_.anchorPoint", nlohmann::json()), sprite_.anchorPoint);
}

void BorderLine::ToJson(nlohmann::json& data) {

    data["sprite_.size.x"] = sprite_.size.x;
    data["playerToDistance_"] = playerToDistance_;
    to_json(data["sprite_.anchorPoint"], sprite_.anchorPoint);
}