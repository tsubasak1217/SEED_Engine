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
    std::unique_ptr<Sprite> backEggUI_;

    // 2桁まで
    std::array<std::unique_ptr<Sprite>,2> leftTimeUI_;

    bool isDoubleDigit_ = true;

public:
    Vector2 GetUIPos()const{ return backEggUI_->translate; }
    Vector2 GetUISize()const{ return backEggUI_->size; }
};
