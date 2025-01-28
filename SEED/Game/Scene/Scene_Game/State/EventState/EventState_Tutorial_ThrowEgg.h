#pragma once
#include "EventState_Base.h"

class EventState_Tutorial_ThrowEgg : public EventState_Base {
public:
    EventState_Tutorial_ThrowEgg();
    EventState_Tutorial_ThrowEgg(Scene_Base* pScene);
    ~EventState_Tutorial_ThrowEgg() = default;
    void Initialize()override;
    void Update()override;
    void Draw()override;

private:

};