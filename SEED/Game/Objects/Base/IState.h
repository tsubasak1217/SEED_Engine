#pragma once
#include "../Game/Objects/Base/BaseCharacter.h"
#include "InputManager.h"

class IState{
public:
    IState() = default;
    IState(BaseCharacter* character){ character; }
    virtual ~IState() = default;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void Initialize(BaseCharacter* character){ pCharacter_ = character; }

protected:
    virtual void ManageState() = 0;

protected:
    BaseCharacter* pCharacter_ = nullptr;
};