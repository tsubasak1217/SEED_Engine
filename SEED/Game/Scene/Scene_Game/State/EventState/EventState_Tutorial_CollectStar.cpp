#include "EventState_Tutorial_CollectStar.h"

//lib
#include "../PlayerInput/PlayerInput.h"

EventState_Tutorial_CollectStar::EventState_Tutorial_CollectStar(){}

EventState_Tutorial_CollectStar::EventState_Tutorial_CollectStar(Scene_Base* pScene) : EventState_Tutorial_Base(pScene)
{
    textStepMax_ = 5;
    spriteClipHeightOffset_ = 63.0f * 8;
    textSprite_->clipLT.y = spriteClipHeightOffset_;
}

EventState_Tutorial_CollectStar::~EventState_Tutorial_CollectStar(){
    pPlayer_->SetIsMovable(true);
}

void EventState_Tutorial_CollectStar::Update(){
    EventState_Tutorial_Base::Update();
}

void EventState_Tutorial_CollectStar::Draw(){
    EventState_Tutorial_Base::Draw();
}
