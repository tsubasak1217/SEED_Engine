#pragma once
#include "../IPostProcess.h"
#include "FogParam.h"

class Fog : public IPostProcess{
public:
    Fog();
    ~Fog() override = default;

public:
    void Initialize() override;
    void Update() override;
    void Apply() override;
    void Release() override;
    void StartTransition() override{};
    void EndTransition() override{};

private:
    DxBuffer<FogParams> fogParamsBuffer_; // ブラーのパラメータを格納するバッファ

public:
    void Edit() override;
};