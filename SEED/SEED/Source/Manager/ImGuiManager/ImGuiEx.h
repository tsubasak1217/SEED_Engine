#pragma once
#include <imgui.h>
#include <string>


namespace ImGui{

    /*----------- stringでラベルを渡せるように関数のstr版を追加(使うものを適宜) -----------*/
    // Button系
    bool Button(const std::string& label, const ImVec2& size = ImVec2(0, 0));
    bool SmallButton(const std::string& label);
    bool InvisibleButton(const std::string& str_id, const ImVec2& size, ImGuiButtonFlags flags = 0);
    bool Checkbox(const std::string& label, bool* v);
    bool RadioButton(const std::string& label, bool active);
    bool RadioButton(const std::string& label, int* v, int v_button);
    bool ImageButton(const std::string& str_id, ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
    // Text系
    void Text(const std::string& text);
    void TextColored(const ImVec4& col, const std::string& text);
    void SeparatorText(const std::string& label);
    // 数値操作系
    bool DragFloat(const std::string& label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool DragFloat2(const std::string& label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool DragFloat3(const std::string& label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool DragFloat4(const std::string& label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool DragInt(const std::string& label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool DragInt2(const std::string& label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool DragInt3(const std::string& label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool SliderFloat(const std::string& label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    bool SliderInt(const std::string& label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    bool InputFloat(const std::string& label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
    bool InputInt(const std::string& label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
    // 色編集系
    bool ColorEdit4(const std::string& label, float col[4], ImGuiColorEditFlags flags = 0);
    // その他
    bool CollapsingHeader(const std::string& label, ImGuiTreeNodeFlags flags = 0);


}