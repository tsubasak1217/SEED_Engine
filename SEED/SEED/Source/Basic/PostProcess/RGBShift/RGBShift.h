#pragma once
#include "../IPostProcess.h"

class RGBShift : public IPostProcess{
public:
    RGBShift();
    ~RGBShift() override = default;

public:
    void Initialize() override;
    void Update() override{};
    void Apply() override;
    void Release() override;
    void StartTransition() override{};
    void EndTransition() override{};

private:
    int gap_ = 2;

public:
    void Edit() override;
};