#pragma once
#include "EventState_Base.h"

class EventState_Tutorial_CollectStar : public EventState_Base {
public:
    EventState_Tutorial_CollectStar();
    EventState_Tutorial_CollectStar(Scene_Base* pScene);
    ~EventState_Tutorial_CollectStar() = default;
    void Initialize()override;
    void Update()override;
    void Draw()override;

private:

};