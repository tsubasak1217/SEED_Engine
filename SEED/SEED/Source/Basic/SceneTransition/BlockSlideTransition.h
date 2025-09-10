#pragma once
#include <vector>
#include <numbers>
#include <SEED/Source/Basic/SceneTransition/ISceneTransition.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Structs/Timer.h>

struct TransitionQuad{
    Quad2D quad;
    float appearTime;
    float disappearTime;
    Vector2 startPos;
    Vector2 midPos;
    Vector2 endPos;
};

class BlockSlideTransition : public ISceneTransition{
public:
    BlockSlideTransition() = default;
    ~BlockSlideTransition() = default;

protected:
    void Update()override;
    void Draw()override;

public:

    // 遷移の開始
    void StartTransition(float curSceneExitTime, float newSceneEnterTime)override {
        ISceneTransition::StartTransition(curSceneExitTime, newSceneEnterTime);
        CreateQuadInfo();
    }

    // 六角形の情報セット
    void SetQuadInfo(
        float quadWidth, // 六角形の半径
        const std::vector<Vector4>& colorList = { {1.0f,1.0f,1.0f,1.0f} }// 色リスト
    );

private:
    void CreateQuadInfo();

private:
    // リスト
    std::vector<TransitionQuad> quads_;
    std::vector<Vector4> colorList_;

    // パラメータ
    float quadWidth_;
    float scaleTime = 0.2f;
    bool isExitScene_ = false;
    float appearEndTimeT_ = 0.7f;
};