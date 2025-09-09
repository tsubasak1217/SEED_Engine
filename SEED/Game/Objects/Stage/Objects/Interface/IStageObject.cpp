#include "IStageObject.h"

//============================================================================
//	IStageObject classMethods
//============================================================================

void IStageObject::AppearanceUpdateAnimation(float baseDuration, float spacing, Easing::Type easing, uint32_t colum) {

    // インデックスで待ち時間の間隔を空ける
    appearanceWaitTimer_.duration = spacing * static_cast<float>(colum);
    appearanceWaitTimer_.Update();
    if (!appearanceWaitTimer_.IsFinished()) {
        return;
    }

    // インデックスでずらして補間する
    appearanceTimer_.duration = baseDuration;
    appearanceTimer_.Update();

    // スケーリングして発生させる
    sprite_.transform.scale = std::lerp(0.0f, 1.0f,
        std::clamp(appearanceTimer_.GetEase(easing), 0.0f, 1.0f));
    commonScale_ = std::lerp(0.0f, 1.0f,
        std::clamp(appearanceTimer_.GetEase(easing), 0.0f, 1.0f));

    // 終了したら固定
    if (appearanceTimer_.IsFinishedNow()) {

        sprite_.transform.scale = 1.0f;
    }
}