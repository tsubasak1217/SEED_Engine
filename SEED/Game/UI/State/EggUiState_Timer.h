#pragma once

//parent
#include "IUiState.h"

///stl
//pointer
#include <memory>

/// local
//object
class Egg;

///math
#include "Vector2.h"

class EggUiState_Timer
    :public IUiState{
public:
    EggUiState_Timer(UI* _ui,
                     Egg* _egg,
                     const std::string& _stateName,
                     float _duration,
                     float* _leftTime);
    ~EggUiState_Timer();

    void Initialize()override;
    void Update()override;
    void Draw()override;

    void BeginFrame()override;
    void EndFrame()override;
private:
    Egg* egg_ = nullptr;

    std::unique_ptr<UI> leftTimeNumber_ = nullptr;

    Vector2 uiPos_;
    Vector2 numberUiOffset_ = {0.0f,0.0f};

    Vector2 min_ = {80.0f,80.0f};
    Vector2 max_ = {1200.0f,640.0f};

    float duration_ = 0.f;
    float* leftTime_ = nullptr;

};