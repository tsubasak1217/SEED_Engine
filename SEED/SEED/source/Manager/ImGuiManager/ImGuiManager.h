#pragma once
// local
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Lib/Structs/Transform.h>
#include <SEED/Lib/Structs/Range2D.h>
#include "ImGuiEx.h"
// stl
#include <initializer_list>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;
class SEED;

// 内部で使用する構造体
struct GuizmoInfo{
    Transform* transform = nullptr; // 操作対象のTransform
    Transform2D* transform2D = nullptr; // 操作対象のTransform2D
    Matrix4x4 parentMat = IdentityMat4(); // 親の行列
};

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
    static void RegisterGuizmoItem(Transform* transform,const Matrix4x4& parentMat = IdentityMat4()){
        if(transform != nullptr){
            GuizmoInfo info;
            info.transform = transform;
            info.parentMat = parentMat;
            instance_->guizmoInfo3D_.push_back(info);
        }
    }
    static void RegisterGuizmoItem(Transform2D* transform, const Matrix4x4& parentMat = IdentityMat4()){
        if(transform != nullptr){
            GuizmoInfo info;
            info.transform2D = transform;
            info.parentMat = parentMat;

            instance_->guizmoInfo2D_.push_back(info);
        }
    }
    static bool GetIsInputText() { return instance_->isInputText_; }
    static void SetIsInputText(bool isInputText) { instance_->isInputText_ = isInputText; }
private:
    std::wstring windowTitle_;
    ImGuizmo::OPERATION currentOperation_ = ImGuizmo::TRANSLATE; // 現在の操作モード
    ImDrawList* pDrawList_ = nullptr; // ImGuiの描画リスト
    std::list<GuizmoInfo> guizmoInfo3D_; // ImGuizmoで操作するTransformのリスト
    std::list<GuizmoInfo> guizmoInfo2D_; // ImGuizmoで操作するTransformのリスト
    bool isInputText_=false;
};



///////////////////////////////////////////////////////////////////
// ImGuiのカスタム関数をまとめるための構造体関連
///////////////////////////////////////////////////////////////////

// ImGuiのウィンドウフラグを拡張するための列挙型
typedef int CustomWindowFlag;
enum CustomImWindowFlag_{
    MoveOnly_TitleBar = 1 << 0, // タイトルバーのみドラッグ可能
    SceneManipurate = 1 << 1, // シーンウィンドウを操作できるようにするか
};

struct ImFunc{

    // カスタムウィンドウの開始関数
    static bool CustomBegin(const char* name, CustomWindowFlag customFlag, ImGuiWindowFlags flags = 0);
    // シーン描画ウィンドウの関数
    static ImVec2 SceneWindowBegin(const char* label,const std::string& cameraName = "", CustomWindowFlag flags = MoveOnly_TitleBar, ImGuiWindowFlags normalFlags = 0);
    // フォルダ表示
    static std::string FolderView(
        const std::string& label,
        std::filesystem::path& currentPath,
        bool isFileNameOnly = false,
        std::initializer_list<std::string> filterExts = {""},
        std::filesystem::path rootPath = ""
    );


    // 文字列を折り返す関数
    static std::vector<std::string> WrapTextLines(const std::string& text, float maxWidth, int maxLines = 2);

    // コンボボックスの拡張関数
    template <typename EnumType>
    static bool Combo(const std::string& label, EnumType& currentValue, initializer_list<string> items, int padding = 0);
    template <typename EnumType>
    static bool Combo(const std::string& label, EnumType& currentValue, const char* const* items, int size, int padding = 0);
    template <typename EnumType>
    static bool ComboPair(const std::string& label, EnumType& currentValue, initializer_list<pair<string, EnumType>>items);
    static bool ComboText(const std::string& label, string& str, const vector<string>& items);
    // ビットマスク
    template <typename EnumType>
    static bool BitMask(const std::string& label, EnumType& bit, initializer_list<string> bitNames);

    // inputTextに直接stringを渡せるように
    static bool InputTextMultiLine(const std::string& label, string& str);
    static bool InputText(const std::string& label, string& str);

    // ImGuizmoの操作を行う関数
    static void Guizmo3D(const GuizmoInfo& info, ImDrawList* pDrawList, Range2D rectRange);
    static void Guizmo2D(const GuizmoInfo& info,ImDrawList* pDrawList,Range2D rectRange);

    // シーンウィンドウの範囲を取得する関数
    static const Range2D& GetSceneWindowRange(const std::string& label);

private:
    static inline std::unordered_map<std::string, Range2D> sceneWindowRanges_; // シーンウィンドウの範囲を保存するマップ
};


template<typename EnumType>
inline bool ImFunc::Combo(const std::string& label, EnumType& currentValue, initializer_list<string> items, int padding){
    vector<const char*> cstrItems;
    cstrItems.reserve(items.size());
    for(const auto& item : items){
        cstrItems.push_back(item.c_str()); // 一時的に const char* に変換
    }

    return Combo(label.c_str(), currentValue, cstrItems.data(), static_cast<int>(cstrItems.size()), padding);
}

template<typename EnumType>
inline bool ImFunc::Combo(const std::string& label, EnumType& currentValue, const char* const* items, int size, int padding){

    int currentIndex = static_cast<int>(currentValue) - padding;
    bool changed = ImGui::Combo(label.c_str(), &currentIndex, items, size);
    if(changed){
        //EnumTypeの最初の値を取得
        currentValue = static_cast<EnumType>(currentIndex + padding);
    }
    return changed;
}

template<typename EnumType>
inline bool ImFunc::ComboPair(const std::string& label, EnumType& currentValue, initializer_list<pair<string, EnumType>> items){
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
    bool changed = ImGui::Combo(label.c_str(), &currentIndex, cstrItems.data(), size);
    if(changed){
        // EnumTypeの最初の値を取得
        auto it = items.begin();
        std::advance(it, currentIndex);
        currentValue = it->second; // 選択された値を設定
    }

    return changed;
}


/////////////////////////////////////////////////////////////////
// ビットマスクのチェックボックスを作成
/////////////////////////////////////////////////////////////////
template<typename EnumType>
inline bool ImFunc::BitMask(const std::string& label, EnumType& bit, initializer_list<string> bitNames){
    bool changed = false;
    int bitValue = static_cast<int>(bit); // EnumTypeをintに変換
    ImGui::Text("%s", label.c_str());

    // ビットマスクのチェックボックスを作成
    for(int i = 0; i < static_cast<int>(bitNames.size()); i++){
        bool isChecked = (bitValue & (1 << i)) != 0; // ビットが立っているかチェック
        if(ImGui::Checkbox(bitNames.begin()[i].c_str(), &isChecked)){
            if(isChecked){
                bitValue |= (1 << i); // ビットを立てる
            } else{
                bitValue &= ~(1 << i); // ビットを下げる
            }
            changed = true;
        }
    }

    // 変更があった場合、値を適用
    if(changed){
        bit = static_cast<EnumType>(bitValue);
    }
    
    return changed;
}
