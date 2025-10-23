#pragma once
#include <vector>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Functions/Easing.h>

/// <summary>
/// カーブのチャンネル数
/// </summary>
enum class CurveChannel{
    FLOAT,
    VECTOR2,
    VECTOR3,
    VECTOR4,
};

/// <summary>
/// カーブの編集操作の内容
/// </summary>
enum class Operation{
    None,
    Drag,
    SelectContextMenu,
};

/// <summary>
/// カーブの各ポイントの情報
/// </summary>
struct EditorCurve{
    std::vector<ImVec2> points;
    std::vector<ImVec2> drawPoints_;
    ImColor curveColor_;
};