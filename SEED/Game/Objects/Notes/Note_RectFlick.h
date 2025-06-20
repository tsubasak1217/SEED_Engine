#pragma once
#include <Game/Objects/Notes/Note_Base.h>

class Note_RectFlick : public Note_Base{
public:
    Note_RectFlick();
    ~Note_RectFlick() override;
    void Update() override;
    void Draw(float currentTime, float appearLength) override;
    Judgement::Evaluation Judge(float dif) override;
private:
    bool CheckBit(DIRECTION8 dir) const;
};