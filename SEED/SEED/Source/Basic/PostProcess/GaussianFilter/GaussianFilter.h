#pragma once
#include "../IPostProcess.h"
#include "BlurParam.h"

class GaussianFilter : public IPostProcess{
public:
    GaussianFilter();
    ~GaussianFilter() override = default;

public:
    void Initialize() override;
    void Update() override;
    void Apply() override;
    void Release() override;
    void StartTransition() override{};
    void EndTransition() override{};

private:
    DxBuffer<BlurParams> blurParamsBuffer_; // ブラーのパラメータを格納するバッファ

public:
    void Edit() override;
    nlohmann::json ToJson() override;
    void FromJson(const nlohmann::json& json) override;
};