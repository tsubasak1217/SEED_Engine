#pragma once
#include "EventState_Tutorial_Base.h"
#include "Scene_Game.h"

class EventState_Tutorial_Corpse : public EventState_Tutorial_Base{
public:
    EventState_Tutorial_Corpse();
    EventState_Tutorial_Corpse(Scene_Base* pScene);
    ~EventState_Tutorial_Corpse()override;
};