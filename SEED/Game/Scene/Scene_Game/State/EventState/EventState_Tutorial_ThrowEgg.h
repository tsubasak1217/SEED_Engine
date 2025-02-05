#pragma once
#include "EventState_Tutorial_Base.h"
#include "Scene_Game.h"

class EventState_Tutorial_ThrowEgg : public EventState_Tutorial_Base{
public:
    EventState_Tutorial_ThrowEgg();
    EventState_Tutorial_ThrowEgg(Scene_Base* pScene);
    ~EventState_Tutorial_ThrowEgg();
    void Update()override;
    void Draw()override;
};