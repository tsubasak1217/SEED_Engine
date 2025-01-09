#pragma once

#include "Base/ICharacterState.h"

class Egg;

/// <summary>
/// 卵の追従ステート
/// </summary>
class EggState_Follow :
    public ICharacterState{
public:
    EggState_Follow(BaseCharacter* _egg,
                    BaseObject* _followTarget);
    ~EggState_Follow();
    void Initialize(const std::string& stateName,BaseCharacter* character)override;
    void Update()override;
    void Draw()override;

protected:
    void MoveFollow();
    // ステート管理
    void ManageState()override;
private:
    const Vector3 followOffset_ = Vector3(0.0f,0.0f,0.0f);
    BaseObject* followTarget_ = nullptr;
    const float followSensitivity_ = 0.2f;
};