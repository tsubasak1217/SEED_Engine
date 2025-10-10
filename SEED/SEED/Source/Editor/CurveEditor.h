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
private:
    struct MyCurve : ImCurveEdit::Delegate{
        ImVec2 min = { 0.0f, 0.0f };
        ImVec2 max = { 1.0f, 1.0f };
        std::vector<ImVec2> points = { {0,0}, {0.5f,1}, {1,0} };

        size_t GetCurveCount() override{ return 1; }
        ImVec2& GetMin() override{ return min; }
        ImVec2& GetMax() override{ return max; }
        size_t GetPointCount(size_t) override{ return points.size(); }
        uint32_t GetCurveColor(size_t) override{ return 0xFFFFFFFF; }
        ImVec2* GetPoints(size_t) override{ return points.data(); }

        int EditPoint(size_t, int idx, ImVec2 value) override{
            points[idx] = value;
            return idx;
        }
        void AddPoint(size_t, ImVec2 value) override{
            points.push_back(value);
        }
    };


public:
    CurveEditor() = default;
    ~CurveEditor() = default;
    void EditGUI();

public:
    CurveType curveType = FLOAT;

};