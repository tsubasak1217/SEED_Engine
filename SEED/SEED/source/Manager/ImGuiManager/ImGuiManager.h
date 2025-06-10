#pragma once
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <initializer_list>
#include <string>
#include <unordered_map>


class SEED;

///////////////////////////////////////////////////////////////////
// ImGuiの描画などを行うクラス
///////////////////////////////////////////////////////////////////
class ImGuiManager{
private:
    // プライベートコンストラクタ
    ImGuiManager() = default;
    // コピー禁止
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;
    // シングルトン
    static ImGuiManager* instance_;
    std::wstring windowTitle_;

public:
    static ImGuiManager* GetInstance();
    static void Initialize();
    static void Finalize();

    static void Draw();

    static void PreDraw();
    static void PostDraw();

public:
    static const std::wstring& GetWindowName(){ return instance_->windowTitle_; }
};



///////////////////////////////////////////////////////////////////
// ImGuiのカスタム関数をまとめるための構造体関連
///////////////////////////////////////////////////////////////////

// ImGuiのウィンドウフラグを拡張するための列挙型
enum CustomImWindowFlag{
    MoveOnly_TitleBar = 1 << 0, // タイトルバーのみドラッグ可能
};

struct ImFunc{

    // カスタムウィンドウの開始関数
    static void CustomBegin(const char* name, CustomImWindowFlag customFlag, ImGuiWindowFlags flags = 0);

    // コンボボックスの拡張関数
    template <typename EnumType>
    static bool Combo(const char* label, EnumType& currentValue, std::initializer_list<std::string> items, int padding = 0);
    template <typename EnumType>
    static bool Combo(const char* label, EnumType& currentValue, const char* const* items, int size, int padding = 0);
    static bool ComboText(const char* label, std::string& str, const std::vector<std::string>& items);

    // inputTextに直接stringを渡せるように
    static bool InputText(const char* label, std::string& str);
};



template<typename EnumType>
inline bool ImFunc::Combo(const char* label, EnumType& currentValue, std::initializer_list<std::string> items, int padding){
    std::vector<const char*> cstrItems;
    cstrItems.reserve(items.size());
    for(const auto& item : items){
        cstrItems.push_back(item.c_str()); // 一時的に const char* に変換
    }

    return Combo(label, currentValue, cstrItems.data(), static_cast<int>(cstrItems.size()), padding);
}

template<typename EnumType>
inline bool ImFunc::Combo(const char* label, EnumType& currentValue, const char* const* items, int size, int padding){

    int currentIndex = static_cast<int>(currentValue) - padding;
    bool changed = ImGui::Combo(label, &currentIndex, items, size);
    if(changed){
        //EnumTypeの最初の値を取得
        currentValue = static_cast<EnumType>(currentIndex + padding);
    }
    return changed;
}