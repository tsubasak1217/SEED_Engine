#pragma once
#include "../IPostProcess.h"
#include "../GaussianFilter/BlurParam.h"

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
    void SwapBuffer();

private:
    // 半分の解像度のテクスチャ
    static inline DxResource halfTexture_[2];

    // パラメータ
    DxBuffer<BlurParams> blurParamsBuffer_;
    float resolutionRate_ = 1.0f; // 解像度レート
    float focusDistance_ = 0.1f;
    float focusDepth_ = 0.01f; // フォーカス深度
    float focusRange_ = 0.01f; // フォーカス範囲
    int downSampledWidth_ = kWindowSizeX;
    int downSampledHeight_ = kWindowSizeY;
    int currentBufferIndex_ = 0; // 現在のバッファインデックス

public:
    void Edit() override;
};