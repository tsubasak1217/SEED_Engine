#pragma once
//#include "ImGuizmo.h"
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <initializer_list>


class SEED;

class ImGuiManager{
private:
    // プライベートコンストラクタ
    ImGuiManager() = default;
    // コピー禁止
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;
    // シングルトン
    static ImGuiManager* instance_;

public:
    static ImGuiManager* GetInstance();

    static void Initialize();
    static void Finalize();

    static void Draw();

    static void PreDraw();
    static void PostDraw();

};

// ImGuiのカスタム関数をまとめるための構造体
struct ImFunc{

    // コンボボックスのテンプレート関数
    template <typename EnumType>
    static bool Combo(const char* label, EnumType& currentValue, std::initializer_list<std::string> items,int padding = 0){
        std::vector<const char*> cstrItems;
        cstrItems.reserve(items.size());
        for(const auto& item : items){
            cstrItems.push_back(item.c_str()); // 一時的に const char* に変換
        }

        return Combo(label, currentValue, cstrItems.data(), static_cast<int>(cstrItems.size()),padding);
    }


    template <typename EnumType>
    static bool Combo(const char* label, EnumType& currentValue, const char* const* items,int size,int padding = 0){

        int currentIndex = static_cast<int>(currentValue) - padding;
        bool changed = ImGui::Combo(label, &currentIndex, items, size);
        if(changed){
            //EnumTypeの最初の値を取得
            currentValue = static_cast<EnumType>(currentIndex + padding);
        }
        return changed;
    }

    static bool ComboText(const char* label, std::string& str, const std::vector<std::string>& items){
        int currentIndex = 0;
        int size = static_cast<int>(items.size()); // padding分を除外
        for(int i = 0; i < size; ++i){
            if(items[i] == str){
                currentIndex = i;
                break;
            }
        }
        // ImGui::Comboはconst char*の配列を受け取るので、std::vectorから変換
        std::vector<const char*> itemsCStr;
        itemsCStr.reserve(size);
        for(const auto& item : items){
            itemsCStr.push_back(item.c_str());
        }

        // Comboの実行
        bool changed = ImGui::Combo(label, &currentIndex, itemsCStr.data(), size);
        if(changed && currentIndex >= 0 && currentIndex < size){
            str = items[currentIndex];
        }
        return changed;
    }

    // inputTextに直接stringを渡せるように
    static bool InputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0){
        char buffer[256] = {};
        strncpy_s(buffer, sizeof(buffer), str.c_str(), _TRUNCATE);

        bool changed = ImGui::InputText(label, buffer, sizeof(buffer), flags);
        if(changed){
            str = buffer;
        }
        return changed;
    }
};