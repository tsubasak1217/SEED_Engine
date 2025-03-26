#pragma once
#include <Game/Objects/Base/ICharacterState.h>

class SampleCharacterState_Idle : public ICharacterState{
public:
    SampleCharacterState_Idle() = default;
    SampleCharacterState_Idle(const std::string& stateName, BaseCharacter* player);
    ~SampleCharacterState_Idle()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName, BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

private:

};