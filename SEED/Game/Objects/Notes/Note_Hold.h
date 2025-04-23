#pragma once
#include <Game/Objects/Notes/Note_Base.h>

class Note_Hold : public Note_Base{
public:
    Note_Hold();
    ~Note_Hold() override;
    void Update() override;
    void Draw() override;
    Judgement::Evaluation Judge(float dif) override;

public:
    float holdTime_;
    float totalHoldTime_;
    bool isHold_;
    bool isStackedToHoldList_ = false;// スタックリストに積まれているか
    Judgement::Evaluation headEvaluation_;// 頭の判定
};