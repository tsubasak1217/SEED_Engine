#pragma once
#include "../IPostProcess.h"

class DoF : public IPostProcess{
public:
    DoF();
    ~DoF() override = default;

public:
    void Initialize() override;
    void Update() override;
    void Apply() override;
    void Release() override;
    void StartTransition() override{};
    void EndTransition() override{};

private:
    float resolutionRate_ = 1.0f; // 解像度レート
    float focusDistance_ = 0.1f;
    float focusDepth_ = 0.01f; // フォーカス深度
    float focusRange_ = 0.01f; // フォーカス範囲

public:
    void Edit() override;
};