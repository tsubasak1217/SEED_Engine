#pragma once

// engine
#include "../SEED/lib/structs/Model.h"

// local
#include "../Manager/FieldObjectManager.h"
#include "FieldObject/FieldObjectName.h"

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
public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    FieldEditor();
    FieldEditor(FieldObjectManager& manager);
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
    void AddModel(
        uint32_t modelNameIndex, 
        const Vector3& scale = {2.5f,2.5f,2.5f},
        const Vector3& rotate = { 0.0f,0.0f,0.0f },
        const Vector3& translate = { 0.0f,0.0f,0.0f }
        );


    // 全てのスイッチから特定のドア参照を削除する関数
    void RemoveDoorFromAllSwitches(FieldObject_Door* doorToRemove, const std::vector<FieldObject_Switch*>& allSwitches) const;
    // jsonファイルの読み込み
    void LoadFromJson(const std::string& filePath);
    // jsonファイルへの保存
    void SaveToJson(const std::string& filePath);
    // テクスチャの読み込み
    void LoadFieldModelTexture();
    // マウスで直接オブジェクト選択
    int32_t GetObjectIndexByMouse(std::vector<std::unique_ptr<FieldObject>>& objects);
    // マウスで直接オブジェクト追加
    void AddObjectByMouse(int32_t objectType);
    // オブジェクトのID再割り当て
    void ReassignIDsByType(uint32_t removedType, std::vector<std::unique_ptr<FieldObject>>& objects);

private:
    // id再割り当て
    template <typename T>
    void ReassignIDsForType(std::vector<std::unique_ptr<FieldObject>>& objects, uint32_t startID = 1);



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
    FieldObjectManager& manager_;

    // 管理用
    std::unordered_map<std::string,uint32_t> modelNameMap_;
    TextureMap textureIDs_;
    const std::string jsonPath_ = "resources/jsons/fieldModels/fieldModels.json";


private:// enum
    // fieldObjectName.h に移動しますた
};

////////////////////////////////////////////////////////////////////////
//  テンプレート関数
////////////////////////////////////////////////////////////////////////
template <typename T>
inline void FieldEditor::ReassignIDsForType(std::vector<std::unique_ptr<FieldObject>>& objects, uint32_t startID){
    uint32_t newID = startID;
    for (auto& obj : objects){
        if (auto typed = dynamic_cast< T* >(obj.get())){
            typed->SetFieldObjectID(newID++);
        }
    }
    // 静的カウンタを更新（Tクラスの nextFieldObjectID_ が存在することが前提）
    T::nextFieldObjectID_ = newID;
}

