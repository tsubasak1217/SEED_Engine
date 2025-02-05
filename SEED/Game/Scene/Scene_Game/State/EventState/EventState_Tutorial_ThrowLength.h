#pragma once
#include "EventState_Tutorial_Base.h"
#include "Scene_Game.h"

class EventState_Tutorial_ThrowLength : public EventState_Tutorial_Base{
public:
    EventState_Tutorial_ThrowLength() = default;
    EventState_Tutorial_ThrowLength(Scene_Base* pScene);
    ~EventState_Tutorial_ThrowLength()override;
};