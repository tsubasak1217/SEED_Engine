#pragma once

#include "Base/ICharacterState.h"

class EggState_Break :
    public ICharacterState{
public:
    EggState_Break(BaseCharacter* character);
    EggState_Break() = default;
    ~EggState_Break() = default;

    void Initialize(BaseCharacter* character)override;
    void Update() override;
    void Draw() override;
protected:
    void ManageState() override;

private:
    const float breakTime_ = 1.0f;
    float leftTime_ = 0.0f;
};