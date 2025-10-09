#pragma once
#include "imgui/ImCurveEdit.h"
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

enum CurveType{
    FLOAT,
    VECTOR2,
    VECTOR3,
    VECTOR4,
};

class CurveEditor{
public:
    CurveEditor() = default;
    ~CurveEditor() = default;
    void EditGUI();

public:
};