#pragma once
#include "../IPostProcess.h"

class GrayScale : public IPostProcess{
public:
    GrayScale();
    ~GrayScale() override = default;

public:
    void Initialize() override;
    void Update() override{};
    void Apply() override;
    void Release() override;
    void StartTransition() override{};
    void EndTransition() override{};

public:
    void Edit() override;
    nlohmann::json ToJson() override;
    void FromJson(const nlohmann::json& json) override;
};