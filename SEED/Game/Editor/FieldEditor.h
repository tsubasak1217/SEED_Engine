#pragma once

// engine
#include "../SEED/lib/structs/Model.h"

// local
#include "../Manager/FieldObjectManager.h"

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
        const Vector3& scale = {10.0f,10.0f,10.0f},
        const Vector3& rotate = { 0.0f,0.0f,0.0f },
        const Vector3& translate = { 0.0f,0.0f,0.0f }
        );

    void DeploymentStartAndGoal();

    // jsonファイルの読み込み
    void LoadFromJson(const std::string& filePath);
    // jsonファイルへの保存
    void SaveToJson(const std::string& filePath);
    // テクスチャの読み込み
    void LoadFieldModelTexture();
    // マウスで直接オブジェクト選択
    int32_t SelectObjectByMouse(std::vector<std::unique_ptr<FieldObject>>& objects);

private:
    //===================================================================*/
    //                   private fields
    //===================================================================*/
    std::unordered_map<std::string,uint32_t> modelNameMap_;

    FieldObjectManager& manager_;

    bool isEditing_ = false;

    // テクスチャ管理
    TextureMap textureIDs_;

    const std::string jsonPath_ = "resources/jsons/fieldModels/fieldModels.json";


private:// enum
    enum FIELDMODELNAME{
        FIELDMODEL_GRASSSOIL,
        FIELDMODEL_SOIL,
        FIELDMODEL_SPHERE,
        FIELDMODEL_DOOR,
        FIELDMODEL_START,
        FIELDMODEL_GOAL,
    };
};