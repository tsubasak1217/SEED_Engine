#pragma once
#include "EventState_Base.h"
#include "Scene_Game.h"

class EventState_Tutorial_Goal : public EventState_Base {
public:
    EventState_Tutorial_Goal();
    EventState_Tutorial_Goal(Scene_Base* pScene);
    ~EventState_Tutorial_Goal()override;
    void Initialize()override;
    void Update()override;
    void Draw()override;

private:
    Scene_Game* pGameScene_;
    FollowCamera* pCamera_;
    Player* pPlayer_;

private:
    float time_;
    const int32_t textStepMax_ = 3;
    int32_t textStep_ = 0;

    std::unique_ptr<Sprite> textFieldSprite_;
    std::unique_ptr<Sprite> textSprite_;
};