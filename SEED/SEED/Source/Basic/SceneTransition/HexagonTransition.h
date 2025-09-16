#pragma once
#include <vector>
#include <numbers>
#include <SEED/Source/Basic/SceneTransition/ISceneTransition.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Structs/Timer.h>

struct TransitionHexagon{
    Vector2 pos;
    Vector4 color;
    float appearTime;
    Timer scalingTimer;
};

class HexagonTransition : public ISceneTransition{
public:
    HexagonTransition() = default;
    ~HexagonTransition() = default;

protected:
    void Update()override;
    void Draw()override;

public:

    // 遷移の開始
    void StartTransition(float curSceneExitTime, float newSceneEnterTime)override {
        ISceneTransition::StartTransition(curSceneExitTime, newSceneEnterTime);
        CreateHexagonInfo();
    }

    // 六角形の情報セット
    void SetHexagonInfo(
        float hexagonRadius, // 六角形の半径
        const std::vector<Vector4>& colorList = { {0.0f,0.0f,0.0f,1.0f} }// 色リスト
    );

private:
    void CreateHexagonInfo();

private:
    // リスト
    std::vector<TransitionHexagon> hexagons_;
    std::vector<Vector4> colorList_;

    // パラメータ
    float hexagonRadius_ = 80.0f;
    float angle_ = std::numbers::pi_v<float> / 2.0f;
    float scaleTime = 0.2f;
    bool isExitScene_ = false;
};