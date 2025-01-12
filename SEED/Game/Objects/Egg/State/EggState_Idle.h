#pragma once

#include "Base/ICharacterState.h"

class EggState_Idle
    :public ICharacterState{
public:
    EggState_Idle() = default;
    EggState_Idle(BaseCharacter* _egg);
    ~EggState_Idle();
    void Initialize(const std::string& stateName,
                    BaseCharacter* character)override;
    void Update()override;
    void Draw()override;

protected:
    // ステート管理
    void ManageState()override;
};

