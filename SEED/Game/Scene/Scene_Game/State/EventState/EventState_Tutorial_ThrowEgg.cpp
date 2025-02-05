#include "EventState_Tutorial_ThrowEgg.h"

EventState_Tutorial_ThrowEgg::EventState_Tutorial_ThrowEgg(){
}

EventState_Tutorial_ThrowEgg::EventState_Tutorial_ThrowEgg(Scene_Base* pScene) : EventState_Tutorial_Base(pScene)
{
    spriteClipHeightOffset_ = 63.0f * 13;
    textStepMax_ = 12;
    textSprite_->clipLT.y = spriteClipHeightOffset_;
}

EventState_Tutorial_ThrowEgg::~EventState_Tutorial_ThrowEgg(){
    pPlayer_->SetIsMovable(true);
}


void EventState_Tutorial_ThrowEgg::Update(){
    EventState_Tutorial_Base::Update();
}

void EventState_Tutorial_ThrowEgg::Draw(){
    EventState_Tutorial_Base::Draw();
}
