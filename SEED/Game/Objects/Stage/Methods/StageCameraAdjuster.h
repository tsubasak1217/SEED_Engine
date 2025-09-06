#pragma once
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Source/SEED.h>

class StageCameraAdjuster{
public:
    StageCameraAdjuster() = default;
    ~StageCameraAdjuster() = default;
    void Update();

public://アクセッサ
    void SetStageRange(const Range2D& range);

private:
    Range2D stageRange_;

    // 補間のために使う変数
    Vector3 startScale_;
    Vector3 targetScale_;
    Vector3 startTranslate_;
    Vector3 targetTranslate_;
    float interpolateTime_ = 1.0f;
    float currentTime_ = 0.0f;
};