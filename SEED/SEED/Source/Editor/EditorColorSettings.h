#pragma once
#include <imgui.h>
#include <SEED/Lib/Functions/Math.h>

namespace SEED{
    namespace EditorColor{
        inline ImU32 componentHeader = Methods::Math::IntColor(232, 127, 0, 255);
        inline ImU32 emitterHeader = Methods::Math::IntColor(214, 0, 128, 255);
        inline ImU32 emitterParamHeader = Methods::Math::IntColor(0, 82, 183, 255);
    }
}