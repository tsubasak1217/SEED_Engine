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
    sprite_ = Sprite("DefaultAssets/white.png");
    // テクスチャGHを取得
    transparentTextureGH_ = sprite_.GH;
    opaqueTextureGH_ = TextureManager::LoadTexture("DefaultAssets/checkerBoard.png");

    // 初期状態を設定
    SetDeactivate();
}

void BorderLine::SetActivate() {

    // アクティブ状態を設定
    currentState_ = State::Active;
    // 画像をアクティブにする
    sprite_.GH = opaqueTextureGH_;
    sprite_.color = Vector4(1.0f, 1.0, 1.0f, 1.0f);
}

void BorderLine::SetDeactivate() {

    // 非アクティブにする
    currentState_ = State::Disable;
    // 画像を非アクティブにする
    sprite_.GH = transparentTextureGH_;
    sprite_.color = Vector4(1.0f, 1.0, 1.0f, 0.04f);
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

void BorderLine::Update(const Vector2& translate, float sizeY) {

    // 状態に応じて更新
    switch (currentState_) {
    case BorderLine::State::Disable: {

        // 非アクティブ状態中に常に座標を更新する
        sprite_.translate = Vector2(translate.x, translate.y + offsetTranslateY_);
        // ウィンドウの一番上にサイズを合わせる
        sprite_.size.y = sizeY;
        break;
    }
    case BorderLine::State::Active: {
        break;
    }
    }
}

void BorderLine::Draw() {

    // 描画
    sprite_.Draw();
}

void BorderLine::Edit(const Vector2& playerTranslate) {

    // 現在の状態を表示
    ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));

    ImGui::Separator();

    ImGui::DragFloat("playerToDistance", &playerToDistance_, 0.1f);
    ImGui::DragFloat("offsetTranslateY", &offsetTranslateY_, 0.01f);
    ImGui::DragFloat("spriteSizeX", &sprite_.size.x, 0.1f);
    ImGui::DragFloat2("spriteAnchor", &sprite_.anchorPoint.x, 0.05f);

    // プレイヤーと境界線
    Vector2 left(sprite_.translate.x - playerToDistance_, sprite_.translate.y);
    Vector2 right(sprite_.translate.x + playerToDistance_, sprite_.translate.y);
    if (CheckPlayerToDistance(playerTranslate.x)) {

        SEED::DrawLine2D(left, right, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    } else {

        SEED::DrawLine2D(left, right, Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    }
}

void BorderLine::FromJson(const nlohmann::json& data) {

    sprite_.size.x = data["sprite_.size.x"];
    playerToDistance_ = data.value("playerToDistance_", 16.0f);
    offsetTranslateY_ = data.value("offsetTranslateY_", 16.0f);
    from_json(data.value("sprite_.anchorPoint", nlohmann::json()), sprite_.anchorPoint);
}

void BorderLine::ToJson(nlohmann::json& data) {

    data["sprite_.size.x"] = sprite_.size.x;
    data["playerToDistance_"] = playerToDistance_;
    data["offsetTranslateY_"] = offsetTranslateY_;
    to_json(data["sprite_.anchorPoint"], sprite_.anchorPoint);
}