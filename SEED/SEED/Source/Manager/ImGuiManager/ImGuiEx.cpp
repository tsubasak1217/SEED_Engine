#include "ImGuiEx.h"

namespace ImGui{
    bool ImGui::Button(const std::string& label, const ImVec2& size){
        return ImGui::Button(label.c_str(), size);
    }

    bool ImGui::SmallButton(const std::string& label){
        return ImGui::SmallButton(label.c_str());
    }

    bool ImGui::InvisibleButton(const std::string& str_id, const ImVec2& size, ImGuiButtonFlags flags){
        return ImGui::InvisibleButton(str_id.c_str(), size, flags);
    }

    bool ImGui::Checkbox(const std::string& label, bool* v){
        return ImGui::Checkbox(label.c_str(), v);
    }

    bool ImGui::RadioButton(const std::string& label, bool active){
        return ImGui::RadioButton(label.c_str(), active);
    }

    bool ImGui::RadioButton(const std::string& label, int* v, int v_button){
        return ImGui::RadioButton(label.c_str(), v, v_button);
    }

    bool ImGui::ImageButton(const std::string& str_id, ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& bg_col, const ImVec4& tint_col){
        return ImGui::ImageButton(str_id.c_str(), user_texture_id, size, uv0, uv1, bg_col, tint_col);
    }

    void ImGui::Text(const std::string& text){
        ImGui::Text("%s", text.c_str());
    }

    void ImGui::TextColored(const ImVec4& col, const std::string& text){
        ImGui::TextColored(col, "%s", text.c_str());
    }

    void ImGui::SeparatorText(const std::string& label){
        ImGui::SeparatorText(label.c_str());
    }

    bool ImGui::DragFloat(const std::string& label, float* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags){
        return ImGui::DragFloat(label.c_str(), v, v_speed, v_min, v_max, format, flags);
    }

    bool ImGui::DragFloat2(const std::string& label, float v[2], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags){
        return ImGui::DragFloat2(label.c_str(), v, v_speed, v_min, v_max, format, flags);
    }

    bool ImGui::DragFloat3(const std::string& label, float v[3], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags){
        return ImGui::DragFloat3(label.c_str(), v, v_speed, v_min, v_max, format, flags);
    }

    bool ImGui::DragFloat4(const std::string& label, float v[4], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags){
        return ImGui::DragFloat4(label.c_str(), v, v_speed, v_min, v_max, format, flags);
    }

    bool ImGui::DragInt(const std::string& label, int* v, float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags){
        return ImGui::DragInt(label.c_str(), v, v_speed, v_min, v_max, format, flags);
    }

    bool ImGui::DragInt2(const std::string& label, int v[2], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags){
        return ImGui::DragInt2(label.c_str(), v, v_speed, v_min, v_max, format, flags);
    }

    bool ImGui::DragInt3(const std::string& label, int v[3], float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags){
        return ImGui::DragInt3(label.c_str(), v, v_speed, v_min, v_max, format, flags);
    }

    bool ImGui::SliderFloat(const std::string& label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags){
        return ImGui::SliderFloat(label.c_str(), v, v_min, v_max, format, flags);
    }

    bool ImGui::SliderInt(const std::string& label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags){
        return ImGui::SliderInt(label.c_str(), v, v_min, v_max, format, flags);
    }

    bool ImGui::InputFloat(const std::string& label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags){
        return ImGui::InputFloat(label.c_str(), v, step, step_fast, format, flags);
    }

    bool ImGui::InputInt(const std::string& label, int* v, int step, int step_fast, ImGuiInputTextFlags flags){
        return ImGui::InputInt(label.c_str(), v, step, step_fast, flags);
    }

    bool ImGui::ColorEdit4(const std::string& label, float col[4], ImGuiColorEditFlags flags){
        return ImGui::ColorEdit4(label.c_str(), col, flags);
    }

    bool ImGui::ColorPicker4(const std::string& label, float col[4], ImGuiColorEditFlags flags, const float* ref_col){
        return ImGui::ColorPicker4(label.c_str(), col, flags, ref_col);
    }

    bool ImGui::CollapsingHeader(const std::string& label, ImGuiTreeNodeFlags flags){
        return ImGui::CollapsingHeader(label.c_str(), flags);
    }

    bool ImGui::BeginPopup(const std::string& str_id, ImGuiWindowFlags flags){
        return ImGui::BeginPopup(str_id.c_str(), flags);
    }

    void ImGui::OpenPopup(const std::string& str_id, ImGuiPopupFlags popup_flags){
        ImGui::OpenPopup(str_id.c_str(), popup_flags);
    }

    bool ImGui::BeginPopupContextItem(const std::string& str_id, ImGuiPopupFlags popup_flags){
        return ImGui::BeginPopupContextItem(str_id.c_str(), popup_flags);
    }
}