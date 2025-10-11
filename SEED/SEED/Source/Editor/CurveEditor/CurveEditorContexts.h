#pragma once
#include <vector>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>

enum class CurveChannel{
    FLOAT,
    VECTOR2,
    VECTOR3,
    VECTOR4,
};

enum class Operation{
    None,
    Drag,
    SelectContextMenu,
};

struct EditorCurve{
    std::vector<ImVec2> points;
    std::vector<ImVec2> drawPoints_;
    ImColor curveColor_;
};