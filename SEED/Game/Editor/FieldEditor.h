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

class FieldObject_Door;
class FieldObject_Switch;

class FieldEditor{
    enum class EditorMode{
        AddFieldObject,
        AddEnemy,
    };

    friend class FieldObject_SaveArea;

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

    // jsonファイルへの保存
    void PopupDecideOutputName();
    void SaveToJson(const std::string& filePath,int32_t stageNo,bool isSaveData = false,Player* playerData = nullptr);
    // テクスチャの読み込み
    void LoadFieldModelTexture();
    // マウスで直接オブジェクト選択
    int32_t GetObjectIndexByMouse(std::vector<std::unique_ptr<FieldObject>>& objects);
    // マウスで直接オブジェクト追加
    void AddObjectByMouse(int32_t objectType);

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
    const std::string jsonPath_ = "resources/jsons/Stages/";

    // ステージ管理用
    int32_t edittingStageIndex = 0;

    // 選択中のもの
    int selectedObjIndex_ = -1;    // FieldObjectならここのインデックスを使う
    int selectedFunctionIndex_ = -1; // Functionならここのインデックスを使う
    uint32_t selectItemIdxOnGUI_ = FIELDMODEL_GRASSSOIL; // 選択中のアイテムのインデックス

};

////////////////////////////////////////////////////////////////////////
//  テンプレート関数
////////////////////////////////////////////////////////////////////////


