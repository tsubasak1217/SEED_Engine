#include "EventState_Tutorial_ThrowLength.h"

EventState_Tutorial_ThrowLength::EventState_Tutorial_ThrowLength(Scene_Base* pScene) : EventState_Tutorial_Base(pScene){
    textStepMax_ = 1;
    spriteClipHeightOffset_ = 63.0f * 27;
    textSprite_->clipLT.y = spriteClipHeightOffset_;
}

EventState_Tutorial_ThrowLength::~EventState_Tutorial_ThrowLength(){
    pPlayer_->SetIsMovable(true);
}
