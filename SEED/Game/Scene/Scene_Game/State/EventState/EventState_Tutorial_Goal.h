#pragma once
#include "EventState_Tutorial_Base.h"
#include "Scene_Game.h"

class EventState_Tutorial_Goal : public EventState_Tutorial_Base{
public:
    EventState_Tutorial_Goal();
    EventState_Tutorial_Goal(Scene_Base* pScene);
    ~EventState_Tutorial_Goal()override;
    void Update()override;
    void Draw()override;
};