#include "IStageObject.h"

//============================================================================
//	IStageObject classMethods
//============================================================================

void IStageObject::AppearanceUpdateAnimation(float baseDuration, float spacing, Easing::Type easing, uint32_t colum) {

    // 空白ブロックに重なっている
    if (isSetStopAppearance_) {

        commonScale_ = 0.0f;
        sprite_.transform.scale = 0.0f;
        owner_->SetWorldScale(0.0f);
        return;
    }

    // 出現時はマスター倍率更新を掛けない
    masterScaleOrder_ = false;

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
        masterScaleOrder_ = true;// マスター倍率更新を再開
        sprite_.transform.scale = 1.0f;
    }
}

// マスター倍率の更新
void IStageObject::MasterScaleUpdate(float timeScale){
    if(!masterScaleOrder_){return;}
    masterScaleTimer_.Update(timeScale);

    // マスター倍率を適用
    sprite_.transform.scale = masterScaleTimer_.GetEase(Easing::OutBack);
    commonScale_ = sprite_.transform.scale.x;
}
