#include "NextStageTransition.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	NextStageTransition classMethods
//============================================================================

void NextStageTransition::StartTransition(
    float curSceneExitTime, float newSceneEnterTime) {

    ISceneTransition::StartTransition(curSceneExitTime, newSceneEnterTime);
    // 使用する形状の初期化
    CreateQuad();
    isExitScene_ = false;
}

void NextStageTransition::SetParam(float stripHeight, float appearEndTimeT, const Vector4& color) {

    stripHeight_ = stripHeight;
    appearEndTimeT_ = appearEndTimeT;

    if (colorList_.empty()) {
        colorList_.push_back(color);
    }
}

void NextStageTransition::Update() {

    if (!isExitScene_) {

        // 時間を進める
        curSceneExitTimer_.Update();

        const float t0 = curSceneExitTimer_.currentTime / curSceneExitTimer_.duration;
        for (auto& quad : quads_) {

            const float phase = quad.appearTime / curSceneExitTimer_.duration;
            float progress = (t0 - phase) / appearEndTimeT_;
            progress = std::clamp(progress, 0.0f, 1.0f);
            const float ease = EaseInOutCubic(progress);
            quad.quad.translate = MyMath::Lerp(quad.startPos, quad.midPos, ease);
            quad.quad.color.w = EaseInExpo(progress);
        }

        // 遷移完了、シーンを遷移させる
        if (curSceneExitTimer_.IsFinishedNow()) {

            isExitScene_ = true;
        }
    } else {

        // 時間を進める
        newSceneEnterTimer_.Update();

        for (auto& quad : quads_) {

            if (newSceneEnterTimer_.currentTime < quad.disappearTime) {
                continue;
            }

            const float progress =
                (newSceneEnterTimer_.currentTime - quad.disappearTime) /
                (newSceneEnterTimer_.duration * appearEndTimeT_);
            const float ease = EaseOutCubic(std::clamp(progress, 0.0f, 1.0f));
            quad.quad.translate = MyMath::Lerp(quad.midPos, quad.endPos, ease);
        }
    }
}

void NextStageTransition::Draw() {

    // 全ての矩形を描画
    for (auto& quad : quads_) {

        SEED::DrawQuad2D(quad.quad);
    }
}

void NextStageTransition::CreateQuad() {

    quads_.clear();

    // 画面高さをストライプ太さで割った行数
    const int num = static_cast<int>(std::ceil(kWindowSize.y / stripHeight_)) + 1;
    Quad2D base = Quad2D();
    base.localVertex[0] = Vector2(-kWindowSize.x, -stripHeight_ / 2.0f);
    base.localVertex[1] = Vector2(0.0f, -stripHeight_ / 2.0f);
    base.localVertex[2] = Vector2(-kWindowSize.x, stripHeight_ / 2.0f);
    base.localVertex[3] = Vector2(0.0f, stripHeight_ / 2.0f);
    base.lightingType = LIGHTINGTYPE_NONE;
    base.isApplyViewMat = false;
    base.layer = 300;
    base.GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");

    // 行ごとの時間オフセット
    const float timeWidthExit = (curSceneExitTimer_.duration * (1.0f - appearEndTimeT_)) / num;
    const float timeWidthEnter = (newSceneEnterTimer_.duration * (1.0f - appearEndTimeT_)) / num;

    for (int i = 0; i < num; ++i) {

        TransitionQuad quad;
        quad.quad = base;
        // 行配置（上から下へ等間隔）
        quad.quad.translate = Vector2(0.0f, 1.0f) * stripHeight_ * static_cast<float>(i);
        quad.quad.color = colorList_.empty() ? Vector4(1.0f)
            : colorList_[i % static_cast<int>(colorList_.size())];

        // 移動位置を設定
        quad.startPos = quad.quad.translate;
        quad.midPos = quad.startPos + Vector2(kWindowSize.x, 0.0f);
        quad.endPos = quad.startPos + Vector2(kWindowSize.x * 2.0f, 0.0f);

        // 行ごとの出現、消えるタイミングを設定
        quad.appearTime = timeWidthExit * i;
        quad.disappearTime = timeWidthEnter * i;
        quads_.push_back(quad);
    }
}