#pragma once
#include "SampleCharacterState_Move.h"

class SampleCharacterState_Jump : public SampleCharacterState_Move{
public:
    SampleCharacterState_Jump() = default;
    SampleCharacterState_Jump(const std::string& stateName,BaseCharacter* player);
    ~SampleCharacterState_Jump()override;
    void Update()override;
    void Draw()override;
    void Initialize(const std::string& stateName,BaseCharacter* player)override;

protected:
    // ステート管理
    void ManageState()override;

private:
    float jumpPower_ = 0.0f;
};