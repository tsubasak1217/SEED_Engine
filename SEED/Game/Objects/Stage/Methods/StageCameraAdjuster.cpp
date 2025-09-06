#include "StageCameraAdjuster.h"
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>
#include <Environment/Environment.h>

void StageCameraAdjuster::Update(){

    if(currentTime_ >= interpolateTime_){ return; }

    auto* camera = SEED::GetMainCamera();
    if(!camera) return;

    // ステージ範囲から幅・高さを計算
    float stageWidth = stageRange_.max.x - stageRange_.min.x;
    float stageHeight = stageRange_.max.y - stageRange_.min.y;

    // ステージの中心を求める
    Vector2 stageCenter = {
        (stageRange_.min.x + stageRange_.max.x) * 0.5f,
        (stageRange_.min.y + stageRange_.max.y) * 0.5f
    };

    // ウィンドウサイズ
    Vector2 windowSize = kWindowSize;

    // スケール計算（横・縦それぞれで何倍必要か）
    float scaleX = stageWidth / windowSize.x;
    float scaleY = stageHeight / windowSize.y;

    // 収めるために大きい方を選ぶ
    float scale = (std::max)(scaleX, scaleY);

    // ターゲットスケールを設定
    targetScale_ = { scale, scale, 1.0f };

    // ターゲット座標を設定
    Vector2 offset = kWindowSize * 0.5f * scale;
    stageCenter -= offset;
    targetTranslate_ = { stageCenter.x, stageCenter.y, 0.0f };

    // トランスフォームを補間
    currentTime_ = std::clamp(currentTime_ + ClockManager::DeltaTime(), 0.0f, interpolateTime_);
    float t = currentTime_ / interpolateTime_;
    float easedT = EaseInOutExpo(t);
    Vector3 newScale = MyMath::Lerp(startScale_, targetScale_, easedT);
    Vector3 newTranslate = MyMath::Lerp(startTranslate_, targetTranslate_, easedT);
    camera->SetScale(newScale);
    camera->SetTranslation(newTranslate);
}

void StageCameraAdjuster::SetStageRange(const Range2D& range){
    stageRange_ = range;
    currentTime_ = 0.0f;

    auto* camera = SEED::GetMainCamera();
    startScale_ = camera->GetScale();
    startTranslate_ = camera->GetTranslation();
}
