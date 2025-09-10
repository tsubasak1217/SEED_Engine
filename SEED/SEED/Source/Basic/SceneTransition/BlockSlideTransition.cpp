#include "BlockSlideTransition.h"
#include <SEED/Lib/Functions/MyFunc/Easing.h>
#include <Environment/Environment.h>


void BlockSlideTransition::Update() {

    if (!isExitScene_) {
        // 現在のシーン終了タイマー
        curSceneExitTimer_.Update();

        for (auto& quad : quads_) {
            if (quad.appearTime > curSceneExitTimer_.currentTime) {
                continue;
            }

            float progress = (curSceneExitTimer_.currentTime - quad.appearTime) / (curSceneExitTimer_.duration * appearEndTimeT_);
            float ease = EaseOutCubic(std::clamp(progress, 0.0f, 1.0f));
            quad.quad.translate = MyMath::Lerp(quad.startPos, quad.midPos, ease);
        }

        // 終わった
        if (curSceneExitTimer_.IsFinishedNow()) {
            isExitScene_ = true;
        }

    } else {
        // 次のシーン開始タイマー
        newSceneEnterTimer_.Update();

        for (auto& quad : quads_) {
            if (quad.disappearTime > newSceneEnterTimer_.currentTime) {
                continue;
            }
            float progress = (newSceneEnterTimer_.currentTime - quad.disappearTime) / (newSceneEnterTimer_.duration * appearEndTimeT_);
            float ease = EaseOutCubic(std::clamp(progress, 0.0f, 1.0f));
            quad.quad.translate = MyMath::Lerp(quad.midPos, quad.endPos, ease);
        }
    }
}

void BlockSlideTransition::Draw() {

    // 矩形描画
    for (auto& quad : quads_) {
        SEED::DrawQuad2D(quad.quad);
    }
}


void BlockSlideTransition::SetQuadInfo(float quadWidth, const std::vector<Vector4>& colorList) {
    // 六角形の半径を設定
    quadWidth_ = quadWidth;

    // 進行方向を設定

    // 色のリストを設定
    colorList_ = colorList;

    if (colorList_.empty()) {
        colorList_.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
    }
}


// 画面に敷き詰める六角形の情報を作成
void BlockSlideTransition::CreateQuadInfo() {

    quads_.clear();

    int num = int(std::ceil(kWindowSize.x / quadWidth_)) + 1;

    Quad2D q = Quad2D();
    q.localVertex[0] = { -quadWidth_ / 2, -kWindowSize.y };
    q.localVertex[1] = { quadWidth_ / 2, -kWindowSize.y };
    q.localVertex[2] = { -quadWidth_ / 2, 0.0f };
    q.localVertex[3] = { quadWidth_ / 2, 0.0f };
    q.isApplyViewMat = false;
    q.lightingType = LIGHTINGTYPE_NONE;
    q.layer = 300;
    q.GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");

    float timeWidth = (curSceneExitTimer_.duration * (1.0f - appearEndTimeT_)) / num;
    float timeWidth2 = (newSceneEnterTimer_.duration * (1.0f - appearEndTimeT_)) / num;

    for (int i = 0; i < num; i++) {
        TransitionQuad quad;
        quad.quad = q;// 頂点情報をコピー
        quad.quad.translate = Vector2(1.0f, 0.0f) * quadWidth_ * float(i);
        quad.quad.color = colorList_[i % (int)colorList_.size()];

        // 移動位置を設定
        quad.startPos = quad.quad.translate;
        quad.midPos = quad.startPos + Vector2(0.0f, kWindowSize.y);
        quad.endPos = quad.startPos + Vector2(0.0f, kWindowSize.y * 2);

        // 順番に出現するように時間を設定
        quad.appearTime = timeWidth * i;
        quad.disappearTime = timeWidth2 * i;

        // 追加
        quads_.push_back(quad);
    }
}
