#pragma once
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
    static bool Combo(const char* label, EnumType& currentValue, std::initializer_list<std::string> items){
        std::vector<const char*> cstrItems;
        cstrItems.reserve(items.size());
        for(const auto& item : items){
            cstrItems.push_back(item.c_str()); // 一時的に const char* に変換
        }

        int currentIndex = static_cast<int>(currentValue);
        bool changed = ImGui::Combo(label, &currentIndex, cstrItems.data(), static_cast<int>(cstrItems.size()));
        if(changed){
            currentValue = static_cast<EnumType>(currentIndex);
        }
        return changed;
    }
};