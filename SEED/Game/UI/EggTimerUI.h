#pragma once

///stl
#include <memory>

///local
#include "Sprite.h"

class EggTimerUI{
public:
    EggTimerUI();
    ~EggTimerUI();

    void Initialize();
    void Update(float _leftTime);
    void Draw();

private:
    std::unique_ptr<Sprite> backEggUI_;
    std::unique_ptr<Sprite> leftTimeUI_;
};
