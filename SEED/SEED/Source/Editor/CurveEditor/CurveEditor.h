#pragma once
#include <SEED/Source/Editor/CurveEditor/CurveEditorContexts.h>
#include <string>
#include <json.hpp>


class CurveEditor{
public:
    CurveEditor() = default;
    ~CurveEditor() = default;
    void EditGUI();

private:
    void Initialize();

private:
    void EditCurves();
    void DrawBG();
    void DrawUnitBorder();
    void DecideOperation();
    void ContextMenuSelect();
    void DragPoint();
    void AddPoint();
    void DrawCurve(const EditorCurve& curve, const ImColor& color);
    void DrawSpline(const std::vector<ImVec2>& points, const ImColor& color,int32_t subdivision = 20);
    void DrawBezier(const std::vector<ImVec2>& points, const ImColor& color, int32_t subdivision = 20);

private:
    nlohmann::json ToJson() const;
    void FromJson(const nlohmann::json& json);

private:
    // パラメーター----------------------------------------
    CurveChannel curveChannel_;
    Easing::Type curveType_;
    std::array<EditorCurve,4> curves_;
    ImRect rect_;
    float pointRadius = 5.0f;
    float unitValueBorder_ = 1.0f;// 1.0と見なすボーダー値

    // 編集用変数------------------------------------------
    //ポイント編集用
    bool isInitialized_ = false;
    int32_t curveIdx_ = 0;
    int selectedPointIndex_ = -1;
    ImVec2 clickedMousePos_;
    ImVec2 originalPointPos_;
    Operation operation_ = Operation::None;
    // ボーダー編集用
    bool isDraggingBorder_ = false;
    //ウィンドウ用
    ImVec2  bottomCursorPos_;
    float editorScale_ = 0.8f;
    std::string tag_;
    //出力用
    std::string outputFilename_;
};