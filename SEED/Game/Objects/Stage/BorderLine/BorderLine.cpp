#include "BorderLine.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <Environment/Environment.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	BorderLine classMethods
//============================================================================

void BorderLine::Initialize() {

    // スプライトを初期化
    sprite_ = Sprite("DefaultAssets/checkerBoard.png");

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

void BorderLine::Edit() {

    // 現在の状態を表示
    ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));

    ImGui::Separator();

    ImGui::DragFloat("spriteSizeX", &sprite_.size.x, 0.1f);
    ImGui::DragFloat2("spriteAnchor", &sprite_.anchorPoint.x, 0.05f);
}

void BorderLine::FromJson(const nlohmann::json& data) {

    sprite_.size.x = data["sprite_.size.x"];
    from_json(data.value("sprite_.anchorPoint", nlohmann::json()), sprite_.anchorPoint);
}

void BorderLine::ToJson(nlohmann::json& data) {

    data["sprite_.size.x"] = sprite_.size.x;
    to_json(data["sprite_.anchorPoint"], sprite_.anchorPoint);
}