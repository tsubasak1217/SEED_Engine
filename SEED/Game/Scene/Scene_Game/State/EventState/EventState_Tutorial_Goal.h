#pragma once
#include "EventState_Base.h"

class EventState_Tutorial_Goal : public EventState_Base {
public:
    EventState_Tutorial_Goal();
    EventState_Tutorial_Goal(Scene_Base* pScene);
    ~EventState_Tutorial_Goal() = default;
    void Initialize()override;
    void Update()override;
    void Draw()override;

private:

};