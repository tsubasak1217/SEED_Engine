#include "FieldEditor.h"
#include "../SEED/external/imgui/imgui.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

FieldEditor::FieldEditor(){
    fieldModel_.clear();
    modelNames_.clear();
}

void FieldEditor::Initialize(){
    // 利用可能なモデル名を設定
    modelNames_ = {"cube", "sphere"};

    LoadFromJson(jsonPath);
}

void FieldEditor::Update(){
    for (auto& model : fieldModel_){
        model->Update();
    }
}

void FieldEditor::Draw(){
    for (auto& model : fieldModel_){
        model->Draw();
    }
}

void FieldEditor::AddModel(const std::string& modelName){
    auto newModel = std::make_unique<Model>(modelName);
    fieldModel_.emplace_back(std::move(newModel));
}

void FieldEditor::LoadFromJson(const std::string& filePath){
    std::ifstream file(filePath);
    if (!file.is_open()){
        return;
    }

    json jsonData;
    file >> jsonData; // JSONデータを読み込む
    file.close();

    // 現在のモデルリストをクリア
    fieldModel_.clear();

    // JSONデータからモデルをロード
    if (jsonData.contains("models")){
        for (const auto& modelJson : jsonData["models"]){
            std::string name = modelJson.value("name", "default_model.obj");
            Vector3 position = {0.0f, 0.0f, 0.0f};
            Vector3 scale = {1.0f, 1.0f, 1.0f};
            Vector3 rotation = {0.0f, 0.0f, 0.0f};

            if (modelJson.contains("position")){
                position.x = modelJson["position"].at(0);
                position.y = modelJson["position"].at(1);
                position.z = modelJson["position"].at(2);
            }
            if (modelJson.contains("scale")){
                scale.x = modelJson["scale"].at(0);
                scale.y = modelJson["scale"].at(1);
                scale.z = modelJson["scale"].at(2);
            }
            if (modelJson.contains("rotation")){
                rotation.x = modelJson["rotation"].at(0);
                rotation.y = modelJson["rotation"].at(1);
                rotation.z = modelJson["rotation"].at(2);
            }

            // モデルを作成してプロパティを設定
            AddModel(name);
            if (!fieldModel_.empty()){
                auto& model = fieldModel_.back();
                model->translate_ = position;
                model->scale_ = scale;
                model->rotate_ = rotation;
            }
        }
    } else{
    }
}

void FieldEditor::SaveToJson(const std::string& filePath){
    try{
        // ファイルのパスからディレクトリ部分を抽出
        std::filesystem::path path(filePath);
        std::filesystem::path directory = path.parent_path();

        // ディレクトリが存在しない場合は作成
        if (!directory.empty() && !std::filesystem::exists(directory)){
            std::filesystem::create_directories(directory);
        }

        json jsonData;

        for (size_t i = 0; i < fieldModel_.size(); ++i){
            const auto& model = fieldModel_[i];
            json modelJson = {
                {"name", model->modelName_},
                {"position", {model->translate_.x, model->translate_.y, model->translate_.z}},
                {"scale", {model->scale_.x, model->scale_.y, model->scale_.z}},
                {"rotation", {model->rotate_.x, model->rotate_.y, model->rotate_.z}}
            };
            jsonData["models"].push_back(modelJson);
        }

        // ファイルに保存
        std::ofstream file(filePath);
        if (file.is_open()){
            file << jsonData.dump(4); // JSONデータを整形して保存
            file.close();
        } else{
        }
    } catch ([[maybe_unused]]const std::exception& e){
    }
}

void FieldEditor::ShowImGui(){
#ifdef _DEBUG
    ImGui::Begin("Field Editor");

    // モデル選択ドロップダウン
    static int selectedModelNameIndex = 0;
    if (!modelNames_.empty()){
        std::vector<const char*> modelNameCStrs;
        for (const auto& name : modelNames_){
            modelNameCStrs.push_back(name.c_str());
        }
        ImGui::Text("Select Model to Add:");
        ImGui::Combo("##ModelSelector", &selectedModelNameIndex, modelNameCStrs.data(), static_cast< int >(modelNameCStrs.size()));
    }

    // モデル追加ボタン
    if (ImGui::Button("Add Selected Model")){
        if (selectedModelNameIndex >= 0 && selectedModelNameIndex < static_cast< int >(modelNames_.size())){
            auto it = std::next(modelNames_.begin(), selectedModelNameIndex);
            AddModel(*it + ".obj");
            if (!fieldModel_.empty()){
                fieldModel_.back()->translate_ = Vector3(0.0f, 0.0f, 0.0f);
                fieldModel_.back()->scale_ = Vector3(1.0f, 1.0f, 1.0f);
                fieldModel_.back()->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
            }
        }
    }

    // 保存ボタン
    ImGui::SameLine();
    if (ImGui::Button("Save Models")){
        SaveToJson(jsonPath);
    }

    ImGui::Separator();

    // モデルリスト
    ImGui::BeginChild("Model List", ImVec2(200, 300), true);
    ImGui::Text("Model List:");
    for (int i = 0; i < fieldModel_.size(); ++i){
        std::string label = "Model " + std::to_string(i);

        if (selectedModelNameIndex == i){
            fieldModel_[i]->color_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.0f, 1.0f));
            if (ImGui::Selectable(label.c_str(), true)){
                selectedModelNameIndex = -1;
            }
            ImGui::PopStyleColor(2);
        } else{
                fieldModel_[i]->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            if (ImGui::Selectable(label.c_str(), false)){

                selectedModelNameIndex = i;
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // モデル編集エリア
    ImGui::BeginChild("Model Editor", ImVec2(250, 300), true);
    if (selectedModelNameIndex >= 0 && selectedModelNameIndex < fieldModel_.size()){
        auto& model = fieldModel_[selectedModelNameIndex];
        ImGui::Text("Editing Model %d", selectedModelNameIndex);
        ImGui::Separator();

        Vector3 position = model->translate_;
        Vector3 scale = model->scale_;
        Vector3 rotation = model->rotate_;

        ImGui::DragFloat3("Position", &position.x, 0.1f);
        ImGui::DragFloat3("Scale", &scale.x, 0.1f);
        ImGui::DragFloat3("Rotation", &rotation.x, 1.0f);

        model->translate_ = position;
        model->scale_ = scale;
        model->rotate_ = rotation;

        if (ImGui::Button("Remove Selected Model")){
            fieldModel_.erase(fieldModel_.begin() + selectedModelNameIndex);
            selectedModelNameIndex = -1;
        }
    } else{
        ImGui::Text("No Model Selected");
    }
    ImGui::EndChild();

    ImGui::End();
#endif // _DEBUG
}
