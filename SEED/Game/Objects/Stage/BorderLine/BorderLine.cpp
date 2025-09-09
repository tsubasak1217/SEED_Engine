#include "BorderLine.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>

// imgui
#include <SEED/External/imgui/imgui.h>

//============================================================================
//	BorderLine classMethods
//============================================================================

void BorderLine::Initialize() {

    // スプライトを初期化
    sprite_ = Sprite("DefaultAssets/white.png");
    sprite_.layer = 21;

    // テクスチャGHを取得
    transparentTextureGH_ = sprite_.GH;
    opaqueTextureGH_ = TextureManager::LoadTexture("Scene_Game/StageObject/borderLine.png");
    initialSize_ = SEED::GetImageSize(L"Scene_Game/StageObject/borderLine.png");

    // 初期状態を設定
    SetDeactivate();
}

void BorderLine::SetActivate() {

    // アクティブ状態を設定
    currentState_ = State::Active;
    // 画像をアクティブにする
    sprite_.GH = opaqueTextureGH_;
    sprite_.color = MyMath::FloatColor(255, 43, 245, 255);
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
    float distance = playerTranslate.x - sprite_.transform.translate.x;
    bool checkDistance = std::fabs(distance) < playerToDistance_;
    // 距離がそもそも遠ければfalseを返す
    if (!checkDistance) {
        return false;
    }
    tileSize;

    // プレイヤーの座標がスプライトの座標よりも高ければ
    return playerTranslate.y <= sprite_.transform.translate.y;
}

bool BorderLine::CanTransitionDisable(const Vector2& playerTranslate, float tileSize) const {

    return IsActive() && CheckPlayerToDistance(playerTranslate, tileSize);
}

void BorderLine::Update(const Vector2& translate, float sizeY, float tileSize) {

    // 状態に応じて更新
    switch (currentState_) {
    case BorderLine::State::Disable: {

        // 座標補間、サイズを更新
        UpdateSprite(translate, sizeY, tileSize);
        break;
    }
    case BorderLine::State::Active: {

        // サイズからscaleを計算して更新
        Vector2 scale = sprite_.size / initialSize_;
        Vector2 exScale = { 0.25f,0.25f };
        // scaleXに応じてscaleYを調整
        float initialAspect = initialSize_.y / initialSize_.x;
        float currentAspect = (sprite_.size.y / scale.y) / sprite_.size.x;
        float aspectRatio = currentAspect / initialAspect;
        scale.y *= aspectRatio;

        // スクロール値を更新
        scrollValue_ += 1.0f * ClockManager::DeltaTime();

        // uvTransformを設定
        sprite_.uvTransform.translate.y = scrollValue_;
        sprite_.uvTransform.scale = { exScale.x, scale.y * exScale.y };

        //シェイク処理
        ShakeBorderLine();

        break;
    }
    }
}

void BorderLine::UpdateSprite(const Vector2& translate, float sizeY, float tileSize) {

    // Y座標とサイズは補間なしで設定
    const float targetX = translate.x;
    sprite_.transform.translate.y = translate.y + offsetTranslateY_ + tileSize;
    sprite_.size.y = sizeY + tileSize;

    // 補間が必要になったら値をリセットして補間する
    const bool needRestart = (!lerpXParam_.running) || (std::fabs(lerpXParam_.targetX - targetX) > 1e-3f);

    // 補間開始時の値を設定する
    if (needRestart) {

        // 開始時のX座標を設定する
        lerpXParam_.startX = sprite_.transform.translate.x;
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
        sprite_.transform.translate.x = lerpXParam_.startX + (lerpXParam_.targetX - lerpXParam_.startX) * easedT;

        // 補間が完了したら補間処理を終了する
        if (1.0f <= lerpT) {

            // 座標を固定
            sprite_.transform.translate.x = lerpXParam_.targetX;
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

    ImGui::SeparatorText("ShakeLine");
    ImGui::Checkbox("isShaking", &isShaking_);
    ImGui::DragFloat("shakeAmount", &shakeAmount_, 0.1f);
    ImGui::DragFloat("shakeDuration", &shakeDuration_, 0.1f);

    // プレイヤーと境界線
    Vector2 left(sprite_.transform.translate.x - playerToDistance_, sprite_.transform.translate.y);
    Vector2 right(sprite_.transform.translate.x + playerToDistance_, sprite_.transform.translate.y);
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

void BorderLine::ShakeBorderLine() {

    // シェイクフラグが立っていなければ処理しない
    if (isShaking_ == false) return;

    
    // 初回開始時に位置を保存
    if (elapsedTime_ == 0.0f) {
        shakeStartPosX_ = sprite_.transform.translate.x;
    }
    elapsedTime_ += ClockManager::DeltaTime();
    if (elapsedTime_ < shakeDuration_) {
        // 経過時間に応じて振動させる
        float progress = elapsedTime_ / shakeDuration_; 

        //振幅を徐々に減少させる
        float decay = 1.0f - progress;

        float wave = std::sin(progress * 100.0f); // 振動回数を調整
        sprite_.transform.translate.x = shakeStartPosX_ + wave * shakeAmount_ * decay;
    }
    else {
        sprite_.transform.translate.x = shakeStartPosX_;
        elapsedTime_ = 0.0f;
        isShaking_ = false;
    }
}
