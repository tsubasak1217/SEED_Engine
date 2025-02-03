#pragma once

//parent
#include "Base/ICharacterState.h"

//object
#include "../UI/EggTimerUI.h"

class EggState_Break :
    public ICharacterState{
public:
    EggState_Break(BaseCharacter* character);
    EggState_Break(BaseCharacter* character,float _breakTime);
    ~EggState_Break();

    void Initialize(const std::string& stateName,BaseCharacter* character)override;
    void Update() override;
    void Draw() override;
protected:
    void ManageState() override;

private:
    BaseCharacter* deadPlayer_ = nullptr;

    std::unique_ptr<EggTimerUI> timerUi_ = nullptr;

    float breakTime_ = 1.0f;
    float leftTime_ = 0.0f;
};