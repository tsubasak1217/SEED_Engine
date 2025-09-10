#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/SceneTransition/ISceneTransition.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Structs/Timer.h>

// c++
#include <vector>
#include <numbers>

//============================================================================
//	NextStageTransition class
//============================================================================
class NextStageTransition :
    public ISceneTransition {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    NextStageTransition() = default;
    ~NextStageTransition() = default;

    void Update()override;
    void Draw()override;

    // 遷移の開始
    void StartTransition(float curSceneExitTime, float newSceneEnterTime) override;

    //--------- accessor -----------------------------------------------------

    void SetParam(float stripHeight, float appearEndTimeT, const Vector4& color);

    bool IsFinishedBegin() const { return curSceneExitTimer_.IsFinished(); }
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- structure ----------------------------------------------------

    // 矩形
    struct TransitionQuad {

        Quad2D quad;
        float appearTime;
        float disappearTime;
        Vector2 startPos;
        Vector2 midPos;
        Vector2 endPos;
    };

    //--------- variables ----------------------------------------------------

     // ストライプリスト
    std::vector<TransitionQuad> quads_;
    std::vector<Vector4> colorList_;

    // パラメータ
    float stripHeight_ = 40.0f;
    float appearEndTimeT_ = 0.7f;
    bool isExitScene_ = false;

    //--------- functions ----------------------------------------------------

    // helper
    void CreateQuad();
};