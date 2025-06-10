#pragma once
// local
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Lib/Structs/Transform.h>
#include <SEED/Lib/Structs/Range2D.h>
// stl
#include <initializer_list>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;
class SEED;

///////////////////////////////////////////////////////////////////
// ImGuiの描画などを行うクラス
///////////////////////////////////////////////////////////////////
class ImGuiManager{
    friend struct ImFunc;
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

public:
    static const std::wstring& GetWindowName(){ return instance_->windowTitle_; }
    static void RegisterGuizmoItem(Transform* transform,bool isUseQuaternion){
        if(transform != nullptr){
            instance_->guizmoTransforms_.push_back({ transform,isUseQuaternion });
        }
    }

private:
    std::wstring windowTitle_;
    ImGuizmo::OPERATION currentOperation_ = ImGuizmo::TRANSLATE; // 現在の操作モード
    ImDrawList* pDrawList_ = nullptr; // ImGuiの描画リスト
    std::list<std::pair<Transform*,bool>> guizmoTransforms_; // ImGuizmoで操作するTransformのリスト
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
    static bool Combo(const char* label, EnumType& currentValue, initializer_list<string> items, int padding = 0);
    template <typename EnumType>
    static bool Combo(const char* label, EnumType& currentValue, const char* const* items, int size, int padding = 0);
    template <typename EnumType>
    static bool ComboPair(const char* label, EnumType& currentValue, initializer_list<pair<string, EnumType>>items);
    static bool ComboText(const char* label, string& str, const vector<string>& items);

    // inputTextに直接stringを渡せるように
    static bool InputText(const char* label, string& str);

    // ImGuizmoの操作を行う関数
    static void Guizmo(Transform* transform,ImDrawList* pDrawList,Range2D rectRange,bool isUseQuaternion);
};



template<typename EnumType>
inline bool ImFunc::Combo(const char* label, EnumType& currentValue, initializer_list<string> items, int padding){
    vector<const char*> cstrItems;
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

template<typename EnumType>
inline bool ImFunc::ComboPair(const char* label, EnumType& currentValue, initializer_list<pair<string, EnumType>> items){
    // 一時的に const char* に変換
    vector<const char*> cstrItems;
    cstrItems.reserve(items.size());
    int size = static_cast<int>(items.size());
    for(const auto& item : items){
        cstrItems.push_back(item.first.c_str());
    }

    // pairの値から currentValue のインデックスを取得
    int currentIndex = 0;
    for(const auto& item : items){
        if(item.second == currentValue){
            break;
        }
        currentIndex++;
    }

    // 選択した要素に結びつけられている値にvalueを設定
    bool changed = ImGui::Combo(label, &currentIndex, cstrItems.data(), size);
    if(changed){
        // EnumTypeの最初の値を取得
        auto it = items.begin();
        std::advance(it, currentIndex);
        currentValue = it->second; // 選択された値を設定
    }

    return changed;
}
