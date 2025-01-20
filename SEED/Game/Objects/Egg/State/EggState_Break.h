#pragma once

#include "Base/ICharacterState.h"

class EggState_Break :
    public ICharacterState{
public:
    EggState_Break(BaseCharacter* character,bool breakToNextStage = false);
    ~EggState_Break() = default;

    void Initialize(const std::string& stateName,BaseCharacter* character)override;
    void Update() override;
    void Draw() override;
protected:
    void ManageState() override;

private:
    BaseCharacter* deadPlayer_ = nullptr;
    bool breakToNextStage_ = false;

    float breakTime_ = 1.0f;
    float leftTime_ = 0.0f;
};