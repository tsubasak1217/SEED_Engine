#pragma once
#include "Base/ICharacterState.h"

class PlayerState_Idle : public ICharacterState{
public:
    PlayerState_Idle() = default;
    PlayerState_Idle(const std::string& stateName, BaseCharacter* player);
    ~PlayerState_Idle()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName, BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

private:

};