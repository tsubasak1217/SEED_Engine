#pragma once
#include "EventState_Tutorial_Base.h"
#include "Scene_Game.h"

class EventState_Tutorial_EatEnemy : public EventState_Tutorial_Base{
public:
    EventState_Tutorial_EatEnemy();
    EventState_Tutorial_EatEnemy(Scene_Base* pScene);
    ~EventState_Tutorial_EatEnemy()override;
    void Update()override;
    void Draw()override;
};