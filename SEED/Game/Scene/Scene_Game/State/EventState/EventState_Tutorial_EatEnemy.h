#pragma once
#include "EventState_Base.h"

class EventState_Tutorial_EatEnemy : public EventState_Base {
public:
    EventState_Tutorial_EatEnemy();
    EventState_Tutorial_EatEnemy(Scene_Base* pScene);
    ~EventState_Tutorial_EatEnemy() = default;
    void Initialize()override;
    void Update()override;
    void Draw()override;

private:

};