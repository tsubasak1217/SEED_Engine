#pragma once
#include <SEED/Lib/Structs/TextBox.h>
#include <cstdint>

struct ComboObject{
    int32_t comboCount = 0;
    TextBox2D comboText;
    ComboObject();
    void Draw() const;
};