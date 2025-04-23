#pragma once
#include <Game/Objects/Notes/Note_Base.h>

class Note_SideFlick : public Note_Base{
public:
    Note_SideFlick();
    ~Note_SideFlick() override;
    void Update() override;
    void Draw(float currentTime, float appearLength) override;
    Judgement::Evaluation Judge(float dif) override;

public:
    LR flickDirection_ = LR::NONE;// フリックの方向
};