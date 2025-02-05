#pragma once

///stl
#include <memory>
#include <array>

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
    // 2桁まで
    std::array<std::unique_ptr<Sprite>,2> leftTimeUI_;

    bool isDoubleDigit_ = true;

public:
    Vector2 GetUIPos()const{ return leftTimeUI_[0]->translate; }
    Vector2 GetUISize()const{ return leftTimeUI_[0]->size; }
};
