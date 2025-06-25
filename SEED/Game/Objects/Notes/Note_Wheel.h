#pragma once
#include <Game/Objects/Notes/Note_Base.h>

class Note_Wheel : public Note_Base{
public:
    Note_Wheel();
    ~Note_Wheel() override;
    void Update() override;
    void Draw(float currentTime, float appearLength) override;
    Judgement::Evaluation Judge(float dif) override;

public:
    UpDown direction_ = UpDown::NONE;
    uint32_t wheelDirectionGH_;
    uint32_t wheelAuraGH_;
    Vector4 upDownColor_[2];
};