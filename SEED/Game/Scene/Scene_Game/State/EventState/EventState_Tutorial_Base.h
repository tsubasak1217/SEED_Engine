#pragma once
#include "EventState_Base.h"
#include "Scene_Game.h"
#include "../PlayerInput/PlayerInput.h"

class EventState_Tutorial_Base : public EventState_Base{
public:
    EventState_Tutorial_Base();
    EventState_Tutorial_Base(Scene_Base* pScene);
    virtual ~EventState_Tutorial_Base()override;
    virtual void Initialize(Scene_Base* pScene);
    virtual void Update()override;
    virtual void Draw()override;

protected:
    Scene_Game* pGameScene_;
    FollowCamera* pCamera_;
    Player* pPlayer_;

protected:
    float time_;
    float kTime_;
    int32_t textStepMax_;
    int32_t textStep_;
    float spriteClipHeightOffset_;

    static std::unique_ptr<Sprite> textFieldSprite_;
    static std::unique_ptr<Sprite> textSprite_;
    static std::unique_ptr<Sprite> skipButtonUI_;
    static std::unique_ptr<Sprite> progressButtonUI_;
};