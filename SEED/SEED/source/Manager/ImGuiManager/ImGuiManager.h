#pragma once
// local
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Lib/Structs/Transform.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Source/Editor/EditorColorSettings.h>
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
    //===================================================================================
    // Guizmoで操作するTransformを登録
    //===================================================================================
    static void RegisterGuizmoItem(Transform* transform, const Matrix4x4& parentMat = IdentityMat4()){
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

public:
    //===================================================================================
    // アクセッサ
    //===================================================================================
    static const std::wstring& GetWindowName(){ return instance_->windowTitle_; }
    static bool GetIsInputNow(){ return instance_->isInputNow_; }
    static void SetIsInputNow(bool isInputNow){ instance_->isInputNow_ = isInputNow; }

    //===================================================================================
    // ヘルパー関数
    //===================================================================================
    static void OpenExplorerMenu(const std::string& itemPath,const std::string& menuName);
private:
    void ExprolerMenu();

private:
    //===================================================================================
    // メンバ変数
    //===================================================================================
    std::wstring windowTitle_;
    ImGuizmo::OPERATION currentOperation_ = ImGuizmo::TRANSLATE; // 現在の操作モード
    ImDrawList* pDrawList_ = nullptr; // ImGuiの描画リスト
    std::list<GuizmoInfo> guizmoInfo3D_; // ImGuizmoで操作するTransformのリスト
    std::list<GuizmoInfo> guizmoInfo2D_; // ImGuizmoで操作するTransformのリスト
    bool isInputNow_ = false;
    std::string explorerItemPath_;
    bool explolerMenuOpenOrder_ = false; // エクスプローラーメニューを開く指示
    std::string menuName_;
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
    static bool CustomBegin(const std::string& name, CustomWindowFlag customFlag, ImGuiWindowFlags flags = 0);
    // シーン描画ウィンドウの関数
    static ImVec2 SceneWindowBegin(const std::string& label, const std::string& cameraName = "", CustomWindowFlag flags = MoveOnly_TitleBar, ImGuiWindowFlags normalFlags = 0);
    // フォルダ表示
    static std::string FolderView(
        const std::string& label,
        std::filesystem::path& currentPath,
        bool isFileNameOnly = false,
        std::initializer_list<std::string> filterExts = { "" },
        std::filesystem::path rootPath = "",
        bool returnDirectoryName = false
    );
    // アクティブボタンの表示
    static bool ActivateImageButton(const std::string& label,bool& isActive, ImTextureID activeIcon, ImTextureID inactiveIcon, const ImVec2& size = ImVec2(30, 30));
    // 再生バーの表示
    static bool PlayBar(const std::string& label, Timer & timer);
    // 折りたたみヘッダーの拡張関数
    static bool CollapsingHeader(const std::string& label, const ImU32& color = 0, ImGuiTreeNodeFlags flags = 0);
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
    // ラジオボタンの拡張関数
    template <typename EnumType>
    static bool RadioButton(const std::string& label, EnumType& currentValue, initializer_list<string> items, int padding = 0);
    template <typename EnumType>
    static bool RadioButton(const std::string& label, EnumType& currentValue, const char* const* items, int size, int padding = 0);
    // ドラッグ＆ドロップの開始関数
    template<typename T>
    static bool BeginDrag(const char* payloadType, const T& value, const std::string& displayName, ImGuiDragDropFlags flags = 0);
    // ドロップされた情報を返す関数
    template <typename T>
    static std::optional<T> GetDroppedData(const char* payloadType, const ImRect& dropArea = ImRect());

    // ビットマスク
    template <typename EnumType>
    static bool BitMask(const std::string& label, EnumType& bit, initializer_list<string> bitNames);

    // inputTextに直接stringを渡せるように
    static bool InputTextMultiLine(const std::string& label, string& str, ImGuiInputTextFlags flags = 0,const ImVec2& size = ImVec2(0, 0));
    static bool InputText(const std::string& label, string& str);

    // ImGuizmoの操作を行う関数
    static void Guizmo3D(const GuizmoInfo& info, ImDrawList* pDrawList, Range2D rectRange);
    static void Guizmo2D(const GuizmoInfo& info, ImDrawList* pDrawList, Range2D rectRange);

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
    bool changed = false;
    bool isOpen = false;
    if(ImGui::BeginCombo(label.c_str(), items[currentIndex])){
        isOpen = true;
        for(int i = 0; i < size; i++){
            const bool is_selected = (currentValue == EnumType(i));
            if(ImGui::Selectable(items[i], is_selected)){
                currentIndex = i, changed = true;
            }
            if(is_selected){
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // 上下キーで選択項目を移動できるようにする
    if(isOpen){
        int direction = Input::IsTriggerKey(DIK_DOWN) - Input::IsTriggerKey(DIK_UP);
        if(direction != 0){
            currentIndex += direction;
            if(currentIndex < padding) currentIndex = padding;
            if(currentIndex >= size) currentIndex = size - 1;
            changed = true;
        }
    }

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


//////////////////////////////////////////////////////////////////
// ラジオボタンの拡張関数
//////////////////////////////////////////////////////////////////
template<typename EnumType>
inline bool ImFunc::RadioButton(const std::string& label, EnumType& currentValue, initializer_list<string> items, int padding){
    bool changed = false;
    ImGui::Text("%s", label.c_str());
    int currentIndex = static_cast<int>(currentValue) - padding;
    // ラジオボタンを作成
    for(int i = 0; i < static_cast<int>(items.size()); i++){
        if(ImGui::RadioButton(items.begin()[i].c_str(), currentIndex == i)){
            currentIndex = i;
            changed = true;
        }
        ImGui::SameLine();
    }
    ImGui::NewLine();
    // 変更があった場合、値を適用
    if(changed){
        currentValue = static_cast<EnumType>(currentIndex + padding);
    }
    return changed;
}

template<typename EnumType>
inline bool ImFunc::RadioButton(const std::string& label, EnumType& currentValue, const char* const* items, int size, int padding){
    bool changed = false;
    ImGui::Text("%s", label.c_str());
    int currentIndex = static_cast<int>(currentValue) - padding;
    // ラジオボタンを作成
    for(int i = 0; i < size; i++){
        if(ImGui::RadioButton(items[i], currentIndex == i)){
            currentIndex = i;
            changed = true;
        }
        ImGui::SameLine();
    }
    ImGui::NewLine();
    // 変更があった場合、値を適用
    if(changed){
        currentValue = static_cast<EnumType>(currentIndex + padding);
    }
    return changed;
}

//////////////////////////////////////////////////////////////////
// ドラッグ＆ドロップの開始関数
//////////////////////////////////////////////////////////////////
template<typename T>
inline bool ImFunc::BeginDrag(const char* payloadType, const T& value, const std::string& displayName, ImGuiDragDropFlags flags){
    if(ImGui::BeginDragDropSource(flags)){
        if constexpr(std::is_same_v<T, std::string>){
            std::string temp = value; // 寿命保証
            ImGui::SetDragDropPayload(payloadType, temp.c_str(), temp.size() + 1);
            ImGui::TextUnformatted(displayName.c_str());
        } else if constexpr(std::is_same_v<T, const char*>){
            ImGui::SetDragDropPayload(payloadType, value, strlen(value) + 1);
            ImGui::TextUnformatted(displayName.c_str());
        } else{
            ImGui::SetDragDropPayload(payloadType, &value, sizeof(T));
            ImGui::TextUnformatted(displayName.c_str());
        }

        ImGui::EndDragDropSource();
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////
// ドロップされた情報を返す関数
//////////////////////////////////////////////////////////////////
template<typename T>
inline std::optional<T> ImFunc::GetDroppedData(const char* payloadType, const ImRect& dropArea){
    std::optional<T> result = std::nullopt;

    // 領域が設定されていなければ通常のドロップエリアとして扱う
    if(dropArea.GetWidth() == 0.0f && dropArea.GetHeight() == 0.0f){
        if(ImGui::BeginDragDropTarget()){
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType)){
                if constexpr(std::is_same_v<T, std::string>){
                    const char* data = static_cast<const char*>(payload->Data);
                    result = std::string(data);
                } else if constexpr(std::is_same_v<T, const char*> || std::is_same_v<T, char*>){
                    result = static_cast<const char*>(payload->Data);
                } else if(payload->DataSize >= sizeof(T)){
                    result = *static_cast<const T*>(payload->Data);
                }
            }
            ImGui::EndDragDropTarget();
        }
    
    } else{// 領域が設定されていればその範囲内でドロップを受け付ける

        if(ImGui::BeginDragDropTargetCustom(dropArea, ImGui::GetCurrentWindow()->ID)){
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType)){
                if constexpr(std::is_same_v<T, std::string>){
                    const char* data = static_cast<const char*>(payload->Data);
                    result = std::string(data);
                } else if constexpr(std::is_same_v<T, const char*> || std::is_same_v<T, char*>){
                    result = static_cast<const char*>(payload->Data);
                } else if(payload->DataSize >= sizeof(T)){
                    result = *static_cast<const T*>(payload->Data);
                }
            }
            ImGui::EndDragDropTarget();
        }
    }

    return result;
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
