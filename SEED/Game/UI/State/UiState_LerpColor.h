#pragma once
#include "IUiState.h"

//math
#include "Vector4.h"

class UiState_LerpColor :
    public IUiState{
public:
    UiState_LerpColor(UI* _ui,const std::string& _stateName);
    UiState_LerpColor(UI* _ui,const std::string& _stateName,float _elapsedTime);
    ~UiState_LerpColor();

    void Initialize()override;
    void Update()override;
    void Draw()override;

    void BeginFrame()override;
    void EndFrame()override;
private:
    float duration_ = 1.f;
    float elapsedTime_ = 0.f;
    Vector4 startColor_;
    Vector4 endColor_;
};