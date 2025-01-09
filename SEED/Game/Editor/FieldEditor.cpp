#include "FieldEditor.h"

#include "JsonManager/JsonCoordinator.h"

#include "../SEED/external/imgui/imgui.h"
#include "../SEED.h"
#include <nlohmann/json.hpp>
#include <fstream>


FieldEditor::FieldEditor(){
    fieldModel_.clear();
    modelNames_.clear();
}

void FieldEditor::Initialize(){
    // 利用可能なモデル名を設定
    modelNames_ = {"groundCube", "sphere"};

    LoadFromJson(jsonPath);

    LoadFieldModelTexture();

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

void FieldEditor::LoadFieldModelTexture(){
    // resources/textures/fieldModelTextures/ 以下の階層にあるテクスチャを自動で読む
    std::vector<std::string> fileNames;

    // 指定されたディレクトリ内のすべてのファイルを探索
    for (const auto& entry : std::filesystem::directory_iterator("resources/textures/fieldModelTextures/")){
        if (entry.is_regular_file()){ // 通常のファイルのみ取得（ディレクトリを除外）
            // もしファイル名が".png"で終わっていたら
            if (entry.path().extension() == ".png"){
                // ファイル名を追加
                fileNames.push_back("fieldModelTextures/" + entry.path().filename().string()); // ファイル名のみ追加
            }
        }
    }

    // テクスチャの読み込み
    for (const auto& fileName : fileNames){

        // テクスチャの読み込み
        int handle = TextureManager::LoadTexture(fileName);

        // GPUハンドルを取得
        textureIDs_[fileName] =
            ( ImTextureID ) ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, handle).ptr;
    }
}

void FieldEditor::ShowImGui(){
#ifdef _DEBUG
    ImGui::Begin("Field Editor");

    ImGui::Checkbox("isEditing", &isEditing_);

    // 選択モデル関連の静的変数
    static int selectedModelNameIndex = 0;
    static int selectedModelIndex = -1;
    static int lastSelectedModelIdxForTransform = -1;
    static Vector3Int moveChunk {0, 0, 0};
    static Vector3Int scaleChunk {1, 1, 1};

    // モデル一覧画像ボタン表示
    ImGui::Text("Select a Model:");
    for (int i = 0; i < static_cast< int >(modelNames_.size()); ++i){
        const std::string& modelName = *std::next(modelNames_.begin(), i);
        std::string imagePath = "fieldModelTextures/" + modelName + "Image.png";

        auto texIt = textureIDs_.find(imagePath);
        if (texIt != textureIDs_.end()){
            if (ImGui::ImageButton(texIt->second, ImVec2(64, 64))){
                selectedModelIndex = i;
            }
        } else{
            ImGui::Text("Texture not found: %s", imagePath.c_str());
        }
        ImGui::SameLine();
    }
    ImGui::NewLine();

    // モデル追加・保存ボタン
    if (ImGui::Button("Add Selected Model") && selectedModelIndex >= 0 &&
        selectedModelIndex < static_cast< int >(modelNames_.size())){
        auto it = std::next(modelNames_.begin(), selectedModelIndex);
        AddModel(*it + ".obj");
        selectedModelNameIndex = static_cast< int >(fieldModel_.size()) - 1;
        if (!fieldModel_.empty()){
            auto& newModel = fieldModel_.back();
            newModel->translate_ = Vector3(0.0f, 0.0f, 0.0f);
            newModel->scale_ = Vector3(10.0f, 10.0f, 10.0f);
            newModel->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Models")){
        SaveToJson(jsonPath);
    }

    ImGui::Separator();

    // 左側のモデルリスト
    ImGui::BeginChild("Model List", ImVec2(200, 300), true);
    ImGui::Text("Model List:");
    for (int i = 0; i < static_cast< int >(fieldModel_.size()); ++i){
        std::string label = "Model " + std::to_string(i);
        bool isSelected = (selectedModelNameIndex == i);
        if (isSelected){
            fieldModel_[i]->color_ = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.f, 1.f));
            ImGui::Selectable(label.c_str(), true);
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

    // 右側のモデル編集エリア
    ImGui::BeginChild("Model Editor", ImVec2(250, 300), true);
    if (selectedModelNameIndex >= 0 && selectedModelNameIndex < static_cast< int >(fieldModel_.size())){
        auto& model = fieldModel_[selectedModelNameIndex];
        ImGui::Text("Editing Model %d", selectedModelNameIndex);
        ImGui::Separator();

        // チャンク変換操作の初期化
        static const float CHUNK_MOVE = 10.0f;
        static const float CHUNK_SCALE = 10.0f;
        if (selectedModelNameIndex != lastSelectedModelIdxForTransform){
            moveChunk = {0, 0, 0};
            scaleChunk = {
                static_cast< int >(model->scale_.x) / 10,
                static_cast< int >(model->scale_.y) / 10,
                static_cast< int >(model->scale_.z) / 10
            };
            lastSelectedModelIdxForTransform = selectedModelNameIndex;
        }

        ImGui::Text("Chunk Transform");
        Vector3Int tempMove = moveChunk;
        Vector3Int tempScale = scaleChunk;

        ImGui::SliderInt3("Move (chunks)", &tempMove.x, -10, 10);
        ImGui::SliderInt3("Scale (chunks)", &tempScale.x, 1, 10);

        if (tempMove != moveChunk){
            Vector3Int diff = {tempMove.x - moveChunk.x, tempMove.y - moveChunk.y, tempMove.z - moveChunk.z};
            model->translate_.x += diff.x * CHUNK_MOVE;
            model->translate_.y += diff.y * CHUNK_MOVE;
            model->translate_.z += diff.z * CHUNK_MOVE;
            moveChunk = tempMove;
        }
        if (tempScale != scaleChunk){
            Vector3Int diff = {tempScale.x - scaleChunk.x, tempScale.y - scaleChunk.y, tempScale.z - scaleChunk.z};
            model->scale_.x += diff.x * CHUNK_SCALE;
            model->scale_.y += diff.y * CHUNK_SCALE;
            model->scale_.z += diff.z * CHUNK_SCALE;
            scaleChunk = tempScale;
        }

        ImGui::Separator();

        // 位置・スケール・回転の編集
        Vector3 position = model->translate_;
        Vector3 scale = model->scale_;
        Vector3 rotation = model->rotate_;

        ImGui::DragFloat3("Position", &position.x, 0.1f);
        ImGui::DragFloat3("Scale", &scale.x, 0.1f);
        ImGui::DragFloat3("Rotation", &rotation.x, 0.01f);

        model->translate_ = position;
        model->scale_ = scale;
        model->rotate_ = rotation;

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

