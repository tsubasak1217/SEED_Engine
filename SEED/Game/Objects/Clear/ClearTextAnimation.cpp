#include "ClearTextAnimation.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <Game/Manager/AudioDictionary.h>

//============================================================================
//	ClearTextAnimation classMethods
//============================================================================

void ClearTextAnimation::Initialize() {

    // スプライト初期化
    // 上
    topText_ = Sprite("Scene_Game/Clear/Clear_Top.png");
    topText_.anchorPoint = Vector2(0.5f, 1.0f);
    topText_.isApplyViewMat = false;
    topText_.color.w = 0.0f;
    topText_.layer = 20;
    // 下
    bottomText_ = Sprite("Scene_Game/Clear/Clear_Bottom.png");
    bottomText_.anchorPoint = Vector2(0.5f, 0.0f);
    bottomText_.isApplyViewMat = false;
    bottomText_.color.w = 0.0f;
    bottomText_.layer = 20;

    // json適応
    ApplyJson();
}

void ClearTextAnimation::StartIn() {

    // 登場アニメーション開始
    currentState_ = AnimState::In;
    animTimer_.Reset();
}

void ClearTextAnimation::StartOut() {

    // 退場アニメーション開始
    currentState_ = AnimState::Out;
    animTimer_.Reset();
}

void ClearTextAnimation::StopAnim() {

    // 何もさせない
    currentState_ = AnimState::None;
}

void ClearTextAnimation::Update() {

    // 状態に応じて更新
    switch (currentState_) {
    case ClearTextAnimation::AnimState::None:
        break;
    case ClearTextAnimation::AnimState::In:

        // 登場アニメーション中
        UpdateInAnim();
        break;
    case ClearTextAnimation::AnimState::Out:

        // 退場アニメーション中
        UpdateOutAnim();
        break;
    }

    seTimer_.Update();
    if(seTimer_.IsFinishedNow()){
        const float kSEVolume = 0.6f;
        AudioManager::PlayAudio(AudioDictionary::Get("クリアメニュー_バー"), false, kSEVolume);
    }
}

void ClearTextAnimation::UpdateInAnim() {

    // 時間を更新
    animTimer_.Update();
    float easedT = animTimer_.GetEase(inAnimEasing_);

    // クリアの方向を取得
    const float radian = DegreeToRadian(textAngle_);
    const Vector2 direction = Vector2(std::cos(radian), std::sin(radian));

    // 補間先の設定
    // 左右から中心に補間
    Vector2 topFrom = targetTranslate_ - (direction * textDistance_);    // 左
    Vector2 bottomFrom = targetTranslate_ + (direction * textDistance_); // 右

    // 位置を補間
    topText_.transform.translate = MyMath::Lerp(topFrom, targetTranslate_, easedT);
    bottomText_.transform.translate = MyMath::Lerp(bottomFrom, targetTranslate_, easedT);

    // 値を設定
    topText_.transform.rotate = radian;
    topText_.transform.scale = textScale_;
    bottomText_.transform.rotate = radian;
    bottomText_.transform.scale = textScale_;

    // 少し経過してからαを戻す
    if (0.1f < animTimer_.GetProgress()) {

        topText_.color.w = 1.0f;
        bottomText_.color.w = 1.0f;
    }

    // 補間処理終了後待機させる
    if (animTimer_.IsFinished()) {

        // 座標を固定
        topText_.transform.translate = targetTranslate_;
        bottomText_.transform.translate = targetTranslate_;
    }
}

void ClearTextAnimation::UpdateOutAnim() {

    // 時間を更新
    animTimer_.Update();
    float easedT = animTimer_.GetEase(inAnimEasing_);

    // クリアの方向を取得
    const float radian = DegreeToRadian(textAngle_);
    const Vector2 direction = Vector2(std::cos(radian), std::sin(radian));

    // 補間先の設定
    // 中心から外側に補間
    Vector2 topTo = targetTranslate_ + (direction * textDistance_);    // 右
    Vector2 bottomTo = targetTranslate_ - (direction * textDistance_); // 左

    // 位置を補間
    topText_.transform.translate = MyMath::Lerp(targetTranslate_, topTo, easedT);
    bottomText_.transform.translate = MyMath::Lerp(targetTranslate_, bottomTo, easedT);

    // 値を設定
    topText_.transform.rotate = radian;
    topText_.transform.scale = textScale_;
    bottomText_.transform.rotate = radian;
    bottomText_.transform.scale = textScale_;

    // 補間処理終了後シーンに通知する
    if (animTimer_.IsFinished()) {

        // 座標を固定
        topText_.transform.translate = targetTranslate_;
        bottomText_.transform.translate = targetTranslate_;
    }

}

bool ClearTextAnimation::IsFinishedInAnim() const {

    // 登場アニメーション中以外はfalse
    if (currentState_ != AnimState::In) {
        return false;
    }
    return animTimer_.IsFinished();
}

bool ClearTextAnimation::IsFinishedOutAnim() const {

    // 退場アニメーション中以外はfalse
    if (currentState_ != AnimState::Out) {
        return false;
    }
    return animTimer_.IsFinished();
}

void ClearTextAnimation::Draw() {

    // Noneの時以外は描画
    if (currentState_ == AnimState::None) {
        return;
    }

    // 文字描画
    topText_.Draw();
    bottomText_.Draw();
}

void ClearTextAnimation::Edit() {

    if (ImGui::Button("Save Json")) {

        SaveJson();
    }
    ImGui::Separator();
    if (ImGui::Button("StartIn")) {

        StartIn();
    }
    ImGui::SameLine();
    if (ImGui::Button("StartOut")) {

        StartOut();
    }
    ImGui::Separator();

    ImGui::Text("currentState: %s", EnumAdapter<AnimState>::ToString(currentState_));

    ImGui::DragFloat("duration", &animTimer_.duration, 0.01f);
    ImGui::DragFloat2("targetTranslate", &targetTranslate_.x, 1.0f);
    ImGui::DragFloat("textAngle", &textAngle_, 1.0f);
    ImGui::DragFloat("textDistance", &textDistance_, 1.0f);
    if (ImGui::DragFloat("textScale", &textScale_, 0.01f)) {

        topText_.transform.scale = textScale_;
        bottomText_.transform.scale = textScale_;
    }

    EnumAdapter<Easing::Type>::Combo("inAnimEasing", &inAnimEasing_);
    EnumAdapter<Easing::Type>::Combo("outAnimEasing", &outAnimEasing_);
}

void ClearTextAnimation::ApplyJson() {

    nlohmann::json data;
    if (!JsonAdapter::LoadCheck("Scene_Select/clearTextAnimation.json", data)) {
        return;
    }

    from_json(data.value("targetTranslate_", nlohmann::json()), targetTranslate_);

    animTimer_.duration = data.value("animTimer_.duration", 0.0f);
    seTimer_ = Timer(animTimer_.duration);
    textAngle_ = data.value("textAngle_", 0.0f);
    textDistance_ = data.value("textDistance_", 400.0f);
    textScale_ = data.value("textScale_", 1.0f);

    inAnimEasing_ = EnumAdapter<Easing::Type>::FromString(data["inAnimEasing_"]).value();
    outAnimEasing_ = EnumAdapter<Easing::Type>::FromString(data["outAnimEasing_"]).value();
}

void ClearTextAnimation::SaveJson() {

    nlohmann::json data;

    to_json(data["targetTranslate_"], targetTranslate_);

    data["animTimer_.duration"] = animTimer_.duration;
    data["textAngle_"] = textAngle_;
    data["textDistance_"] = textDistance_;
    data["textScale_"] = textScale_;

    data["inAnimEasing_"] = EnumAdapter<Easing::Type>::ToString(inAnimEasing_);
    data["outAnimEasing_"] = EnumAdapter<Easing::Type>::ToString(outAnimEasing_);

    JsonAdapter::Save("Scene_Select/clearTextAnimation.json", data);
}

float ClearTextAnimation::DegreeToRadian(float degree) {

    return degree * (std::numbers::pi_v<float> / 180.0f);
}