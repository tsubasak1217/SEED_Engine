#include "FieldEditor.h"
#include "../SEED/external/imgui/imgui.h"
#include "../SEED.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

FieldEditor::FieldEditor(){
    fieldModel_.clear();
    modelNames_.clear();
}

void FieldEditor::Initialize(){
    // 利用可能なモデル名を設定
    modelNames_ = {"groundCube", "sphere"};

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
    std::string path = "assets/" + modelName;
    auto newModel = std::make_unique<Model>(path);
    newModel->isRotateWithQuaternion_ = false;
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

            // モデル名から "assets/" プレフィックスを取り除く
            std::string modelName = model->modelName_;
            const std::string prefix = "assets/";
            if (modelName.rfind(prefix, 0) == 0){  // modelName が prefix で始まる場合
                modelName = modelName.substr(prefix.length());
            }

            json modelJson = {
                {"name", modelName},
                {"position", {model->translate_.x, model->translate_.y, model->translate_.z}},
                {"scale",    {model->scale_.x, model->scale_.y, model->scale_.z}},
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
    } catch ([[maybe_unused]] const std::exception& e){
    }
}

void FieldEditor::ShowImGui(){
#ifdef _DEBUG
    ImGui::Begin("Field Editor");

    ImGui::Checkbox("isEditing", &isEditing_);

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
                fieldModel_.back()->scale_ = Vector3(10.0f, 10.0f, 10.0f);
                fieldModel_.back()->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
            }
        }
    }

    // 保存ボタンなどの処理
    ImGui::SameLine();
    if (ImGui::Button("Save Models")){
        SaveToJson(jsonPath);
    }

    ImGui::Separator();

    //---------------------------------------------
    // 既存の「Model List」表示
    //---------------------------------------------
    ImGui::BeginChild("Model List", ImVec2(200, 300), true);
    ImGui::Text("Model List:");
    for (int i = 0; i < fieldModel_.size(); ++i){
        std::string label = "Model " + std::to_string(i);

        if (selectedModelNameIndex == i){
            fieldModel_[i]->color_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.0f, 1.0f));
            if (ImGui::Selectable(label.c_str(), true)){
                // すでに選択しているので特に何もしない
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

    //---------------------------------------------
    // 既存のモデル編集エリア
    //---------------------------------------------
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
        ImGui::DragFloat3("Rotation", &rotation.x, 0.01f);

        model->translate_ = position;
        model->scale_ = scale;
        model->rotate_ = rotation;

        static const float CHUNK_MOVE = 10.0f; // 1チャンクで座標10移動
        static const float CHUNK_SCALE = 10.0f; // 1チャンクでスケール10変化

        static int lastSelectedModelIndex = -1;

        static Vector3Int moveChunk = {0, 0, 0};
        static Vector3Int scaleChunk = {1, 1, 1};

        //1チャンクにつき10
        if (selectedModelNameIndex != lastSelectedModelIndex){
            moveChunk = {0, 0, 0};
            scaleChunk = Vector3Int(
                static_cast< int >(model->scale_.x) / 10,
                static_cast< int >(model->scale_.y) / 10,
                static_cast< int >(model->scale_.z) / 10
            );
            lastSelectedModelIndex = selectedModelNameIndex;
        }

        ImGui::Separator();
        ImGui::Text("Dynamic Chunk Transform");

        static Vector3Int oldMove = {0, 0, 0};
        static Vector3Int oldScale = {1, 1, 1};

        Vector3Int tempMove = moveChunk;
        Vector3Int tempScale = scaleChunk;

        ImGui::SliderInt3("Move (chunks)", &tempMove.x, -10, 10);
        ImGui::SliderInt3("Scale (chunks)", &tempScale.x, 1, 10);

        if (tempMove != moveChunk){
            Vector3Int diff = tempMove - moveChunk;

            model->translate_.x += diff.x * CHUNK_MOVE;
            model->translate_.y += diff.y * CHUNK_MOVE;
            model->translate_.z += diff.z * CHUNK_MOVE;

            moveChunk = tempMove;
        }

        if (tempScale != scaleChunk){
            Vector3Int diff = tempScale - scaleChunk;

            model->scale_.x += diff.x * CHUNK_SCALE;
            model->scale_.y += diff.y * CHUNK_SCALE;
            model->scale_.z += diff.z * CHUNK_SCALE;

            scaleChunk = tempScale;
        }

        ImGui::Separator();
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
