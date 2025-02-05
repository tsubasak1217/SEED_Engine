#pragma once
#include "EventState_Tutorial_Base.h"
#include "Scene_Game.h"

class EventState_Tutorial_CollectStar : public EventState_Tutorial_Base{
public:
    EventState_Tutorial_CollectStar();
    EventState_Tutorial_CollectStar(Scene_Base* pScene);
    ~EventState_Tutorial_CollectStar();
    void Update()override;
    void Draw()override;
};