#pragma once

//parent
#include "Base/ICharacterState.h"

//object
#include "../UI/UI.h"

class EggState_Break :
    public ICharacterState{
public:
    EggState_Break(BaseCharacter* character,bool breakToNextStage = false);
    ~EggState_Break();

    void Initialize(const std::string& stateName,BaseCharacter* character)override;
    void Update() override;
    void Draw() override;
protected:
    void ManageState() override;

private:
    BaseCharacter* deadPlayer_ = nullptr;

    std::unique_ptr<UI> timerUi_ = nullptr;

    bool breakToNextStage_ = false;

    // 倍速係数
    float timeFactor_ = 2.0f;

    float breakTime_ = 1.0f;
    float leftTime_ = 0.0f;
};