#pragma once

// engine
#include "../SEED/lib/structs/Model.h"

// lib
#include "../SEED/lib/tensor/Vector3.h"

// c++
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <cstdint>

class FieldEditor{
public:
    //===================================================================*/
    //                   public methods
    //===================================================================*/
    FieldEditor();
    ~FieldEditor() = default;

    void Initialize();
    void Update();
    void Draw();

    // ui/imgui
    void ShowImGui();

    // getter
    bool GetIsEditing()const{ return isEditing_; }

private:
    //===================================================================*/
    //                   private methods
    //===================================================================*/
    void AddModel(const std::string& modelName);
    void LoadFromJson(const std::string& filePath);
    void SaveToJson(const std::string& filePath);

private:
    //===================================================================*/
    //                   private fields
    //===================================================================*/
    std::vector<std::unique_ptr<Model>> fieldModel_ {};
    std::list<std::string> modelNames_ {};

    bool isEditing_ = false;

    const std::string jsonPath = "resources/jsons/fieldModels/fieldModels.json";
};
