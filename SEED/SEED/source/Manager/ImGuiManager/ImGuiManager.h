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

private:
    std::wstring windowTitle_;
    ImGuizmo::OPERATION currentOperation_ = ImGuizmo::TRANSLATE; // 現在の操作モード
    ImDrawList* pDrawList_ = nullptr; // ImGuiの描画リスト
    std::list<GuizmoInfo> guizmoInfo3D_; // ImGuizmoで操作するTransformのリスト
    std::list<GuizmoInfo> guizmoInfo2D_; // ImGuizmoで操作するTransformのリスト
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
        const char* label,
        std::filesystem::path& currentPath,
        bool isFileNameOnly = false,
        std::initializer_list<std::string> filterExts = {""}
    );

    // 文字列を折り返す関数
    static std::vector<std::string> WrapTextLines(const std::string& text, float maxWidth, int maxLines = 2);

    // コンボボックスの拡張関数
    template <typename EnumType>
    static bool Combo(const char* label, EnumType& currentValue, initializer_list<string> items, int padding = 0);
    template <typename EnumType>
    static bool Combo(const char* label, EnumType& currentValue, const char* const* items, int size, int padding = 0);
    template <typename EnumType>
    static bool ComboPair(const char* label, EnumType& currentValue, initializer_list<pair<string, EnumType>>items);
    static bool ComboText(const char* label, string& str, const vector<string>& items);

    // inputTextに直接stringを渡せるように
    static bool InputTextMultiLine(const char* label, string& str);
    static bool InputText(const char* label, string& str);

    // ImGuizmoの操作を行う関数
    static void Guizmo3D(const GuizmoInfo& info, ImDrawList* pDrawList, Range2D rectRange);
    static void Guizmo2D(const GuizmoInfo& info,ImDrawList* pDrawList,Range2D rectRange);
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
