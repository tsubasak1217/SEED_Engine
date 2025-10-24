#pragma once
#include <imgui.h>
#include <SEED/Lib/Functions/MyMath.h>

namespace EditorColor{
    inline ImU32 componentHeader = MyMath::IntColor(232, 127, 0,255);
    inline ImU32 emitterHeader = MyMath::IntColor(214, 0, 128,255);
    inline ImU32 emitterParamHeader = MyMath::IntColor(0, 82, 183,255);
}