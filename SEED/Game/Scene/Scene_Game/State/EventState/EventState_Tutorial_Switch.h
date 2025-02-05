#pragma once
#include "EventState_Tutorial_Base.h"
#include "Scene_Game.h"

class EventState_Tutorial_Switch : public EventState_Tutorial_Base{
public:
    EventState_Tutorial_Switch();
    EventState_Tutorial_Switch(Scene_Base* pScene);
    ~EventState_Tutorial_Switch()override;
};