#pragma once

// engine
#include "../SEED/lib/structs/Model.h"

// local
#include "FieldObject/FieldObjectName.h"
#include "../Manager/StageManager.h"

// lib
#include "../SEED/lib/tensor/Vector3.h"

#include "imgui.h"

// c++
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <cstdint>
#include <unordered_map>

// ImGui用のテクスチャIDなどを保持
using TextureMap = std::unordered_map<std::string, ImTextureID>;

class FieldObject_Door;
class FieldObject_Switch;

class FieldEditor{
    enum class EditorMode{
        AddFieldObject,
        AddEnemy,
    };
public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    FieldEditor();
    FieldEditor(StageManager& manager);
    ~FieldEditor() = default;

    void Initialize();

    // ui/imgui
    void ShowImGui();

    // getter
    bool GetIsEditing()const{ return isEditing_; }

private:
    //===================================================================*/
    //                   private methods
    //===================================================================*/

    // 全てのスイッチから特定のドア参照を削除する関数
    void RemoveDoorFromAllSwitches(FieldObject_Door* doorToRemove, const std::vector<FieldObject_Switch*>& allSwitches) const;
    // jsonファイルへの保存
    void PopupDecideOutputName();
    void SaveToJson(const std::string& filePath,int32_t stageNo);
    // テクスチャの読み込み
    void LoadFieldModelTexture();
    // マウスで直接オブジェクト選択
    int32_t GetObjectIndexByMouse(std::vector<std::unique_ptr<FieldObject>>& objects);
    // マウスで直接オブジェクト追加
    void AddObjectByMouse(int32_t objectType);
    // オブジェクトのID再割り当て
    void ReassignIDsByType(uint32_t removedType, std::vector<std::unique_ptr<FieldObject>>& objects);

    // 敵を配置するフロー用
    void AddEnemyByMouse();
    int32_t GetEnemyIndexByMouse(const std::vector<std::unique_ptr<Enemy>>& enemies);

    // 選択関連
    void UpdateSelectionByMouse(); // FieldObject / Enemy 両方を照合

private:
    //===================================================================*/
    //                   private fields
    //===================================================================*/

    // パラメーター
    const float kBlockSize = 5.0f;
    const float kBlockScale = 2.5f;

    // フラグ
    bool isEditing_ = false;

    // 参照
    StageManager& manager_;

    // 管理用
    std::unordered_map<std::string,uint32_t> modelNameMap_;
    TextureMap textureIDs_;
    const std::string jsonPath_ = "resources/jsons/Stages/";

    // ステージ管理用
    const int kMaxStage = 11;
    int32_t edittingStageIndex = 0;


    // モード切り替え用フラグ
    EditorMode editorMode_ = EditorMode::AddFieldObject;

    // 選択中のもの
    bool selectedIsEnemy_ = false;
    int selectedObjIndex_ = -1;    // FieldObjectならここのインデックスを使う
    int selectedEnemyIndex_ = -1;  // Enemyならここのインデックスを使う
    int selectedFunctionIndex_ = -1; // Functionならここのインデックスを使う
    uint32_t selectItemIdxOnGUI_ = FIELDMODEL_GRASSSOIL; // 選択中のアイテムのインデックス

private:// enum
    // fieldObjectName.h に移動しますた
};

////////////////////////////////////////////////////////////////////////
//  テンプレート関数
////////////////////////////////////////////////////////////////////////


