#pragma once

#include <array>
#include <memory>

#include "Sprite.h"
#include <Vector2.h>

class FieldObject_Switch;

class SwitchWeightUI{
public:
    SwitchWeightUI(FieldObject_Switch* _host);
    ~SwitchWeightUI();

    void Initialize();
    void Update();
    void Draw();

private:
    FieldObject_Switch* host_ = nullptr;

    // 2桁まで
    std::array<std::unique_ptr<Sprite>,2> weightUI_;

    bool isDoubleDigit_ = false;

public:
    Vector2 GetUIPos()const{ return weightUI_[0]->translate; }
    Vector2 GetUISize()const{ return weightUI_[0]->size; }
};

