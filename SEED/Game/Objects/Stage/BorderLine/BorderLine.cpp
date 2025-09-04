#include "BorderLine.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
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

bool BorderLine::CheckPlayerToDistance(const Vector2& playerTranslate, float tileSize) const {

    // 状態がアクティブじゃないときは判定しない
    if (currentState_ == State::Disable) {
        return false;
    }

    // プレイヤーとの距離が近ければtrueを返す
    float distance = playerTranslate.x - sprite_.translate.x;
    bool checkDistance = std::fabs(distance) < playerToDistance_;
    // 距離がそもそも遠ければfalseを返す
    if (!checkDistance) {
        return false;
    }
    // Y座標がほぼ同じならtrueを返す
    float diff = sprite_.translate.y - playerTranslate.y;
    if (std::fabs(diff) < tileSize / 2.0f) {
        return true;
    }
    return false;
}

bool BorderLine::CanTransitionDisable(const Vector2& playerTranslate, float tileSize) const {

    return IsActive() && CheckPlayerToDistance(playerTranslate, tileSize);
}

void BorderLine::Update(const Vector2& translate, float sizeY) {

    // 状態に応じて更新
    switch (currentState_) {
    case BorderLine::State::Disable: {

        // 座標補間、サイズを更新
        UpdateSprite(translate, sizeY);
        break;
    }
    case BorderLine::State::Active: {
        break;
    }
    }
}

void BorderLine::UpdateSprite(const Vector2& translate, float sizeY) {

    // Y座標とサイズは補間なしで設定
    const float targetX = translate.x;
    sprite_.translate.y = translate.y + offsetTranslateY_;
    sprite_.size.y = sizeY;

    // 補間が必要になったら値をリセットして補間する
    const bool needRestart = (!lerpXParam_.running) || (std::fabs(lerpXParam_.targetX - targetX) > 1e-3f);

    // 補間開始時の値を設定する
    if (needRestart) {

        // 開始時のX座標を設定する
        lerpXParam_.startX = sprite_.translate.x;
        lerpXParam_.targetX = targetX;
        // タイマーをリセット
        lerpXParam_.elapsed = 0.0f;
        lerpXParam_.running = true;
    }

    // 補間中の処理
    if (lerpXParam_.running) {

        // 時間経過を進める
        lerpXParam_.elapsed += ClockManager::DeltaTime();
        // 座標を補間する
        float lerpT = (std::min)(1.0f, lerpXParam_.elapsed / lerpXParam_.duration);
        float easedT = Easing::Ease[lerpXParam_.easing](lerpT);
        sprite_.translate.x = lerpXParam_.startX + (lerpXParam_.targetX - lerpXParam_.startX) * easedT;

        // 補間が完了したら補間処理を終了する
        if (1.0f <= lerpT) {

            // 座標を固定
            sprite_.translate.x = lerpXParam_.targetX;
            lerpXParam_.running = false;
        }
    }
}

void BorderLine::Draw() {

    // 描画
    sprite_.Draw();
}

void BorderLine::Edit(const Vector2& playerTranslate, float tileSize) {

    // 現在の状態を表示
    ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));

    ImGui::Separator();

    ImGui::DragFloat("playerToDistance", &playerToDistance_, 0.1f);
    ImGui::DragFloat("offsetTranslateY", &offsetTranslateY_, 0.01f);
    ImGui::DragFloat("spriteSizeX", &sprite_.size.x, 0.1f);
    ImGui::DragFloat2("spriteAnchor", &sprite_.anchorPoint.x, 0.05f);

    ImGui::SeparatorText("Lerp");

    ImGui::DragFloat("lerpXDuration", &lerpXParam_.duration, 0.01f);
    EnumAdapter<Easing::Type>::Combo("LerpEasing", &lerpXParam_.easing);

    // プレイヤーと境界線
    Vector2 left(sprite_.translate.x - playerToDistance_, sprite_.translate.y);
    Vector2 right(sprite_.translate.x + playerToDistance_, sprite_.translate.y);
    if (CheckPlayerToDistance(playerTranslate, tileSize)) {

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

    lerpXParam_.duration = data.value("lerpXDuration_", 0.08f);
    lerpXParam_.easing = EnumAdapter<Easing::Type>::FromString(data.value("lerpXEasing_", "InSine")).value();
}

void BorderLine::ToJson(nlohmann::json& data) {

    data["sprite_.size.x"] = sprite_.size.x;
    data["playerToDistance_"] = playerToDistance_;
    data["offsetTranslateY_"] = offsetTranslateY_;
    to_json(data["sprite_.anchorPoint"], sprite_.anchorPoint);

    data["lerpXDuration_"] = lerpXParam_.duration;
    data["lerpXEasing_"] = EnumAdapter<Easing::Type>::ToString(lerpXParam_.easing);
}