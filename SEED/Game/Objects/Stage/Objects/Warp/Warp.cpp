#include "Warp.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <Game/Objects/Stage/Objects/Warp/Methods/WarpHelper.h>

//============================================================================
//	Warp classMethods
//============================================================================

void Warp::Initialize() {

    // スプライトの初期化
    sprite_ = Sprite("Scene_Game/StageObject/frameRect.png");
    sprite_.anchorPoint = Vector2(0.5f);

    // 初期化値
    currentState_ = State::None;

    // NoneState
    {
        frameAnimTimer_.duration = 1.6f;
        frameSpriteCount_ = 8;
        innerScalingSpacing_ = 0.32f;
        initScale_ = 0.64f;
        scalineEasing_ = Easing::InQuad;
    }
}

void Warp::InitializeFrameSprites() {

    // 全てクリア
    frameSprites_.clear();
    frameAnimTimers_.clear();

    frameSprites_.resize(frameSpriteCount_);
    frameAnimTimers_.resize(frameSpriteCount_);

    for (int index = 0; index < frameSpriteCount_; ++index) {

        Timer& timer = frameAnimTimers_[index];
        Sprite& sprite = frameSprites_[index];

        // スプライトを初期化
        sprite = Sprite("Scene_Game/StageObject/frameRect.png");
        sprite.anchorPoint = Vector2(0.5f);
        sprite.scale = Vector2(initScale_);
        sprite.color = WarpHelper::GetIndexColor(warpIndex_);

        timer.Reset();
        timer.duration = frameAnimTimer_.duration;

        // 一定間隔でずらす
        float phase = std::fmod(innerScalingSpacing_ * index, 1.0f);
        timer.currentTime = timer.duration * phase;
        timer.prevTime = timer.currentTime;
    }
}


void Warp::SetTranslate(const Vector2& translate) {

    sprite_.translate = translate;
    // 初期サイズ
    for (auto& frameSprite : frameSprites_) {

        frameSprite.translate = translate;
    }
}

void Warp::SetSize(const Vector2& size) {

    sprite_.size = size;
    // 初期サイズ
    for (auto& frameSprite : frameSprites_) {

        frameSprite.size = size;
    }
}

void Warp::SetWarpIndex(uint32_t warpIndex) {

    // ワープインデックスを設定
    warpIndex_ = warpIndex;
    // ワープインデックスで色を決定する
    sprite_.color = WarpHelper::GetIndexColor(warpIndex_);

    // アニメーション表示初期化
    InitializeFrameSprites();
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

void Warp::ResetAnimation() {

    // アニメーション表示初期化
    InitializeFrameSprites();
}

void Warp::Update() {

    // 常に行う更新処理
    UpdateAlways();

    // 状態別の更新処理
    UpdateState();
}

void Warp::UpdateState() {

    switch (currentState_) {
    case Warp::State::None:
        break;
    case Warp::State::Notification:
        break;
    case Warp::State::Warping:
        break;
    case Warp::State::WarpNotPossible:
        break;
    case Warp::State::WarpEmpty:
        break;
    }

    // とりあえず常に、入った瞬間出た瞬間は一旦なし
    // 通常状態の更新処理
    UpdateNone();
}

void Warp::UpdateNone() {

    // 空、サイズ以上の場合は処理しない
    if (frameAnimTimers_.empty() || frameSprites_.empty() ||
        frameSprites_.size() < frameSpriteCount_) {
        return;
    }

    // アニメーションタイマーを進める
    for (int index = 0; index < frameSpriteCount_; ++index) {

        Timer& timer = frameAnimTimers_[index];
        Sprite& sprite = frameSprites_[index];

        // 時間を進める
        timer.Update();
        float easedT = timer.GetEase(scalineEasing_);

        // スプライトを一定間隔置きにスケーリングして小さくする
        sprite.scale = std::lerp(initScale_, 0.0f, easedT);

        // 完了したらリスタート
        if (timer.IsFinished()) {

            timer.Reset();
        }
    }
}

void Warp::UpdateAlways() {
}

void Warp::Draw() {

    // アニメーションスプライトを描画
    for (auto& frameSprite : frameSprites_) {

        frameSprite.Draw();
    }
}

void Warp::Edit() {

    // 現在の状態を表示
    ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));
    ImGui::Text("ownType: %s:", EnumAdapter<StageObjectCommonState>::ToString(commonState_));

    if (ImGui::Button("ReStart Param")) {

        InitializeFrameSprites();
    }

    if (ImGui::DragFloat("frameAnimDuration", &frameAnimTimer_.duration, 0.01f)) {
        for (auto& timer : frameAnimTimers_) {

            timer.duration = frameAnimTimer_.duration;
        }
    }
    ImGui::DragInt("frameSpriteCount", &frameSpriteCount_, 1, 1, 128);
    ImGui::DragFloat("innerScalingSpacing", &innerScalingSpacing_, 0.01f);
    ImGui::DragFloat("initScale", &initScale_, 0.01f);
    EnumAdapter<Easing::Type>::Combo("scalineEasing", &scalineEasing_);
}