#pragma once
#include "EventState_Base.h"
#include "Scene_Game.h"

class EventState_Tutorial_CollectStar : public EventState_Base {
public:
    EventState_Tutorial_CollectStar();
    EventState_Tutorial_CollectStar(Scene_Base* pScene);
    ~EventState_Tutorial_CollectStar() = default;
    void Initialize()override;
    void Update()override;
    void Draw()override;

private:
    Scene_Game* pGameScene_;
    FollowCamera* pCamera_;
    Player* pPlayer_;

private:
    float time_;
    const int32_t textStepMax_ = 5;
    int32_t textStep_ = 0;

    std::unique_ptr<Sprite> textFieldSprite_;
    std::unique_ptr<Sprite> textSprite_;
    float spriteClipHeightOffset_ = 63.0f * 8;
};