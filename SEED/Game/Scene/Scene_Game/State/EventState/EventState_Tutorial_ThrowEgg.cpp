#include "EventState_Tutorial_ThrowEgg.h"
#include "FollowCamera.h"

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
    if(FollowCamera* followCamera = dynamic_cast<FollowCamera*>(pPlayer_->GetFollowCamera())){
        followCamera->SetDistance(50.0f);
    }
}