#pragma once
#include <vector>
#include <SEED/Lib/Includes/include_tensors.h>
#include <SEED/Lib/Functions/Easing.h>
#include <SEED/Source/Editor/CurveEditor/CurveEditor.h>
#include <SEED/Source/Editor/CurveEditor/CurveEditorContexts.h>
#include <json.hpp>

using Points = std::vector<Vector2>;

// カーブ構造体
class Curve{
    friend CurveEditor;
public:
    Easing::Type curveType_ = Easing::Type::None;
    CurveChannel channel_ = CurveChannel::FLOAT;

private:
    std::array<Points,4> curves_;
    float unitValueBorder_ = 1.0f;
    CurveEditor editor_;

public:
    Curve();
    void Edit();

public:// アクセッサ
    float GetValue(float t, int channel = 0) const;
    Vector2 GetValue2(float t) const;
    Vector3 GetValue3(float t) const;
    Vector4 GetValue4(float t) const;
    void FromJson(const nlohmann::json& json);
    nlohmann::json ToJson()const;
};