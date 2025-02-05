#include "EventState_Tutorial_Switch.h"

EventState_Tutorial_Switch::EventState_Tutorial_Switch(){
}

EventState_Tutorial_Switch::EventState_Tutorial_Switch(Scene_Base* pScene) : EventState_Tutorial_Base(pScene){
    textStepMax_ = 3;
    spriteClipHeightOffset_ = 63.0f * 28;
    textSprite_->clipLT.y = spriteClipHeightOffset_;
}

EventState_Tutorial_Switch::~EventState_Tutorial_Switch(){
    pPlayer_->SetIsMovable(true);
}
