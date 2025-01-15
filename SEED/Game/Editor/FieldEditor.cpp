#include "FieldEditor.h"

// local
#include "FieldObject/Door/FieldObject_Door.h"
#include "FieldObject/GrassSoil/FieldObject_GrassSoil.h"
#include "FieldObject/Soil/FieldObject_Soil.h"
#include "FieldObject/Sphere/FieldObject_Sphere.h"
#include "FieldObject/Start/FieldObject_Start.h"
#include "FieldObject/Goal/FieldObject_Goal.h"

//engine
#include "../SEED/external/imgui/imgui.h"
#include "../SEED.h"
#include "CollisionManaer/Collision.h"

//lib
#include "JsonManager/JsonCoordinator.h"
#include <nlohmann/json.hpp>
#include <fstream>


////////////////////////////////////////////////////////////////////////////////////////
//  コンストラクタ
////////////////////////////////////////////////////////////////////////////////////////
FieldEditor::FieldEditor(FieldObjectManager& manager)
    : manager_(manager){
    modelNameMap_.clear();
}

////////////////////////////////////////////////////////////////////////////////////////
//  初期化
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::Initialize(){
    // 利用可能なモデル名を設定
    modelNameMap_["GrassSoil"] = FIELDMODEL_GRASSSOIL;
    modelNameMap_["Soil"] = FIELDMODEL_SOIL;
    modelNameMap_["sphere"] = FIELDMODEL_SPHERE;
    modelNameMap_["door"] = FIELDMODEL_DOOR;
    modelNameMap_["start"] = FIELDMODEL_START;
    modelNameMap_["goal"] = FIELDMODEL_GOAL;

    LoadFromJson(jsonPath_);

    LoadFieldModelTexture();

}

////////////////////////////////////////////////////////////////////////////////////////
//  オブジェクトの追加
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::AddModel(
    uint32_t modelNameIndex,
    const Vector3& scale,
    const Vector3& rotate,
    const Vector3& translate
){
    // スタートもしくはゴールの場合、既に存在しているかチェックし、
    // 存在していれば新規追加をキャンセルする
    if(modelNameIndex == FIELDMODEL_START || modelNameIndex == FIELDMODEL_GOAL){
        auto& objects = manager_.GetObjects();
        for(const auto& objPtr : objects){
            FieldObject* obj = objPtr.get();
            if(obj && obj->GetFieldObjectType() == modelNameIndex){
                // 既に同じタイプのオブジェクトが存在する場合、追加をキャンセル
                return;
            }
        }
    }

    // 新規オブジェクトの生成
    std::unique_ptr<FieldObject> newObj = nullptr;

    switch(modelNameIndex){
    case FIELDMODEL_GRASSSOIL:
        newObj = std::make_unique<FieldObject_GrassSoil>();
        break;
    case FIELDMODEL_SOIL:
        newObj = std::make_unique<FieldObject_Soil>();
        break;
    case FIELDMODEL_SPHERE:
        newObj = std::make_unique<FieldObject_Sphere>();
        break;
    case FIELDMODEL_DOOR:
        newObj = std::make_unique<FieldObject_Door>();
        break;
    case FIELDMODEL_START:
        newObj = std::make_unique<FieldObject_Start>();
        break;
    case FIELDMODEL_GOAL:
        newObj = std::make_unique<FieldObject_Goal>();
        break;
    default:
        break;
    }

    if(!newObj) return;  // newObj が生成されなかった場合は何もしない

    // スタートまたはゴールの場合、スケールを1/10に調整
    Vector3 adjustedScale = scale;
    if(modelNameIndex == FIELDMODEL_START || modelNameIndex == FIELDMODEL_GOAL){
        adjustedScale.x = 1.0f;
        adjustedScale.y = 1.0f;
        adjustedScale.z = 1.0f;
    }

    // 初期値の設定
    newObj->SetTranslate(translate);
    newObj->SetScale(adjustedScale);
    newObj->SetRotate(rotate);
    newObj->SetFieldObjectType(modelNameIndex);
    newObj->UpdateMatrix();

    // Manager に登録
    manager_.AddFieldObject(std::move(newObj));
}


////////////////////////////////////////////////////////////////////////////////////////
//  jsonファイルの読み込み
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::LoadFromJson(const std::string& filePath){
    namespace fs = std::filesystem;

    if(!fs::exists(filePath)){
        return;
    }
    std::ifstream file(filePath);
    if(!file.is_open()){
        return;
    }

    nlohmann::json jsonData;
    file >> jsonData;
    file.close();

    // いったん全消去
    manager_.ClearAllFieldObjects();

    // JSON から "models" 配列を読み取り
    if(jsonData.contains("models")){
        for(auto& modelJson : jsonData["models"]){
            std::string name = modelJson.value("name", "default_model.obj");
            uint32_t type = 0;
            Vector3 position{ 0.f, 0.f, 0.f };
            Vector3 scale{ 1.f, 1.f, 1.f };
            Vector3 rotation{ 0.f, 0.f, 0.f };

            if(modelJson.contains("position")){
                position.x = modelJson["position"][0];
                position.y = modelJson["position"][1];
                position.z = modelJson["position"][2];
            }
            if(modelJson.contains("scale")){
                scale.x = modelJson["scale"][0];
                scale.y = modelJson["scale"][1];
                scale.z = modelJson["scale"][2];
            }
            if(modelJson.contains("rotation")){
                rotation.x = modelJson["rotation"][0];
                rotation.y = modelJson["rotation"][1];
                rotation.z = modelJson["rotation"][2];
            }
            if(modelJson.contains("type")){
                type = modelJson["type"];
            }

            // 取得した情報からモデルを追加
            AddModel(type, scale, rotation, position);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//  jsonファイルの保存
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::SaveToJson(const std::string& filePath){
    try{
        namespace fs = std::filesystem;
        fs::path path(filePath);
        auto directory = path.parent_path();

        if(!directory.empty() && !fs::exists(directory)){
            fs::create_directories(directory);
        }

        nlohmann::json jsonData;

        // Managerからオブジェクトを取得
        auto& objects = manager_.GetObjects();
        for(size_t i = 0; i < objects.size(); ++i){
            // dynamic_cast で ModelFieldObject のみ処理
            auto* modelObj = dynamic_cast<FieldObject*>(objects[i].get());
            if(!modelObj) continue;

            // モデル名から "assets/" を外すサンプル
            std::string modelName = modelObj->GetName();
            const std::string prefix = "assets/";
            if(modelName.rfind(prefix, 0) == 0){
                modelName = modelName.substr(prefix.size());
            }


            nlohmann::json modelJson = {
                {"name", modelName},
                {"type", modelObj->GetFieldObjectType()},
                {"position", { modelObj->GetModel()->GetWorldTranslate().x,
                               modelObj->GetModel()->GetWorldTranslate().y,
                               modelObj->GetModel()->GetWorldTranslate().z }},
                {"scale", { modelObj->GetModel()->GetWorldScale().x,
                            modelObj->GetModel()->GetWorldScale().y,
                            modelObj->GetModel()->GetWorldScale().z }},
                {"rotation", { modelObj->GetModel()->GetWorldRotate().x,
                               modelObj->GetModel()->GetWorldRotate().y,
                               modelObj->GetModel()->GetWorldRotate().z }}
            };
            jsonData["models"].push_back(modelJson);
        }

        // JSONファイルに書き込み
        std::ofstream outFile(filePath);
        if(outFile.is_open()){
            outFile << jsonData.dump(4);
            outFile.close();
        }
    } catch([[maybe_unused]] const std::exception& e){
        // エラー処理
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//  textureの読み込み
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::LoadFieldModelTexture(){
    // resources/textures/fieldModelTextures/ 以下の階層にあるテクスチャを自動で読む
    std::vector<std::string> fileNames;

    // 指定されたディレクトリ内のすべてのファイルを探索
    for(const auto& entry : std::filesystem::directory_iterator("resources/textures/fieldModelTextures/")){
        if(entry.is_regular_file()){ // 通常のファイルのみ取得（ディレクトリを除外）
            // もしファイル名が".png"で終わっていたら
            if(entry.path().extension() == ".png"){
                // ファイル名を追加
                fileNames.push_back("fieldModelTextures/" + entry.path().filename().string()); // ファイル名のみ追加
            }
        }
    }

    // テクスチャの読み込み
    for(const auto& fileName : fileNames){

        // テクスチャの読み込み
        int handle = TextureManager::LoadTexture(fileName);

        // GPUハンドルを取得
        textureIDs_[fileName] =
            (ImTextureID)ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, handle).ptr;
    }
}


////////////////////////////////////////////////////////////////////////////////////////
//  マウスで直接オブジェクト選択(indexを取得)
////////////////////////////////////////////////////////////////////////////////////////
int32_t FieldEditor::GetObjectIndexByMouse(std::vector<std::unique_ptr<FieldObject>>& objects){

    // マウスの位置を取得
    Vector2 mousePos = Input::GetMousePosition();
    // マウスの位置からRayを取得
    Line ray = SEED::GetCamera()->GetRay(mousePos);
    // 一番近いオブジェクトを探す用
    float minDist = FLT_MAX;
    int32_t index = -1;

    // 全オブジェクトをチェック
    for(int32_t i = 0; i < objects.size(); ++i){
        auto* obj = objects[i].get();
        if(!obj) continue;

        // オブジェクトのコライダーを取得
        auto& colliders = obj->GetColliders();
        for(auto& collider : colliders){
            // Rayと当たり判定
            if(collider->CheckCollision(ray)){

                // 距離を取得
                float dist = MyMath::Length(collider->GetWoarldTranslate() - ray.origin_);

                // もし距離が一番近い場合、そのオブジェクトを選択
                if(dist < minDist){
                    minDist = dist;
                    index = i;
                }
            }
        }
    }

    return index;
}


////////////////////////////////////////////////////////////////////////////////////////
//  マウスで直接オブジェクト追加
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::AddObjectByMouse(int32_t objectType){
    static Vector3 putPos = { 0.0f,0.0f,0.0f };
    static bool isEdit = false;

    // 基準位置やoffsetをリセット
    if(Input::IsTriggerMouse(MOUSE_BUTTON::RIGHT)){
        isEdit = true;
    }

    // 編集中でない場合は何もしない
    if(!isEdit){ return; }

    // y座標を更新
    putPos.y += kBlockSize * Input::GetMouseWheel();

    // y軸の高さのxz平面を求める
    Quad plane;
    plane.localVertex[0] = { -1000.0f,putPos.y, 1000.0f, };
    plane.localVertex[1] = { 1000.0f,putPos.y, 1000.0f, };
    plane.localVertex[2] = { -1000.0f,putPos.y,-1000.0f, };
    plane.localVertex[3] = { 1000.0f,putPos.y,-1000.0f, };

    // マウスの位置からRayを取得
    Line ray = SEED::GetCamera()->GetRay(Input::GetMousePosition());

    // RayとPlaneの当たり判定
    CollisionData data = Collision::Quad::Line(plane, ray);

    // 衝突点が追加場所
    putPos.x = data.hitPos.value().x;
    putPos.z = data.hitPos.value().z;

    // ブロック単位に丸める
    putPos.x = kBlockSize * int(putPos.x / kBlockSize);
    putPos.y = kBlockSize * int(putPos.y / kBlockSize);
    putPos.z = kBlockSize * int(putPos.z / kBlockSize);

    // 追加予定場所にAABBを表示
    AABB aabb;
    aabb.center = putPos + Vector3(0.0f, -kBlockSize * 0.5f, 0.0f);
    aabb.halfSize = Vector3(kBlockSize * 0.5f, kBlockSize * 0.5f, kBlockSize * 0.5f);
    SEED::DrawAABB(aabb, { 1.0f,1.0f,0.0f,1.0f });

    // マウスのボタンが離されたらオブジェクトを追加
    if(Input::IsReleaseMouse(MOUSE_BUTTON::RIGHT)){
        AddModel(objectType, { kBlockScale, kBlockScale, kBlockScale }, { 0.0f,0.0f,0.0f }, putPos);
        isEdit = false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//  imguiの表示
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::ShowImGui(){
#ifdef _DEBUG
    ImGui::Begin("Field Editor");

    ImGui::Checkbox("isEditing", &isEditing_);

    ImGui::Text("mouseWheel: %d", Input::GetMouseWheel());

    // 選択されているモデルのインデックス
    static int selectedModelIndex = -1;

    // ========== モデルサムネの一覧をボタンで並べる ========== 
    ImGui::Text("Select a Model to Add:");
    int i = 0;
    for(auto& map : modelNameMap_){
        // テクスチャIDを探す
        std::string imageKey = "fieldModelTextures/" + map.first + "Image.png";
        auto it = textureIDs_.find(imageKey);

        if(it != textureIDs_.end()){
            // サムネテクスチャがある場合
            if(ImGui::ImageButton(it->second, ImVec2(64, 64))){
                AddModel(map.second);
            }
        } else{
            // テクスチャがない場合はボタン
            if(ImGui::Button(map.first.c_str(), ImVec2(64, 64))){
                AddModel(map.second);
            }
        }
        ImGui::SameLine();
        i++;
    }
    ImGui::NewLine();

    // ========== マウスで直接オブジェクト追加 ==========

    AddObjectByMouse(FIELDMODEL_GRASSSOIL);

    // ========== 選択したモデルを追加するボタン, Saveボタン ========== 

    if(ImGui::Button("Save Models")){
        SaveToJson(jsonPath_);
    }

    // ========== 現在のオブジェクト一覧(左) と 編集UI(右) ========== 
    ImGui::Separator();

    // Managerから取得
    auto& objects = manager_.GetObjects();

    static int selectedObjIndex = -1;
    ImGui::BeginChild("ModelList", ImVec2(200, 300), true);
    {
        ImGui::Text("Model List:");
        for(int idx = 0; idx < (int)objects.size(); idx++){
            auto* mfObj = dynamic_cast<FieldObject*>(objects[idx].get());
            if(!mfObj) continue;

            // "Model idx"ラベル
            std::string label = "Model " + std::to_string(idx);
            bool isSelected = (selectedObjIndex == idx);

            // 選択中は色を変える(黄色文字とか)
            if(isSelected){
                mfObj->SetColor({ 1.f, 0.f, 0.f, 1.f }); // 選択中=赤色
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.f));
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.f, 1.f));
                ImGui::Selectable(label.c_str(), true);
                ImGui::PopStyleColor(2);
            } else{
                // 非選択時は白
                mfObj->SetColor({ 1.f, 1.f, 1.f, 1.f });
                if(ImGui::Selectable(label.c_str(), false)){
                    selectedObjIndex = idx;
                }
            }
        }
    }

    ImGui::EndChild();

    // マウスでのオブジェクト直接選択
    if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
        if(!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
            selectedObjIndex = GetObjectIndexByMouse(objects);
        }
    }

    ImGui::SameLine();

    // ========== 選択オブジェクトの編集UI ========== 
    ImGui::BeginChild("ModelEditor", ImVec2(250, 300), true);
    {
        if(selectedObjIndex >= 0 && selectedObjIndex < (int)objects.size()){
            auto* mfObj = dynamic_cast<FieldObject*>(objects[selectedObjIndex].get());
            if(mfObj){
                ImGui::Text("Editing Model %d", selectedObjIndex);
                ImGui::Separator();

                // --- 1) チャンク移動/スケール用 変数 ---
                static Vector3Int moveChunk{ 0, 0, 0 };
                static Vector3Int scaleChunk{ 1, 1, 1 };
                static int lastSelectedIndex = -1;

                // 選択が変わったらmoveChunkやscaleChunkを再初期化
                if(selectedObjIndex != lastSelectedIndex){
                    // 現在のモデルからチャンク数を推定して再セット
                    Vector3 curScale = mfObj->GetModel()->GetWorldScale();
                    scaleChunk.x = (int)(curScale.x / 10.f);
                    scaleChunk.y = (int)(curScale.y / 10.f);
                    scaleChunk.z = (int)(curScale.z / 10.f);

                    moveChunk = { 0, 0, 0 };
                    lastSelectedIndex = selectedObjIndex;
                }

                // スライダーを表示
                ImGui::Text("Chunk Transform");
                Vector3Int tempMove = moveChunk;
                ImGui::DragInt3("Move (chunks)", &tempMove.x, 0.1f);

                // 変更があったら実際のPosition / Scaleに反映
                Vector3 pos = mfObj->GetModel()->GetWorldTranslate();
                Vector3 scl = mfObj->GetModel()->GetWorldScale();

                // 移動
                if(tempMove != moveChunk){
                    Vector3Int diff{
                        tempMove.x - moveChunk.x,
                        tempMove.y - moveChunk.y,
                        tempMove.z - moveChunk.z
                    };
                    pos.x += diff.x * kBlockSize;
                    pos.y += diff.y * kBlockSize;
                    pos.z += diff.z * kBlockSize;
                    mfObj->SetTranslate(pos);

                    moveChunk = tempMove;
                }


                ImGui::Separator();

                // --- 2) 細かいTransformの編集(DragFloatなど) ---
                pos = mfObj->GetModel()->GetWorldTranslate();
                scl = mfObj->GetModel()->GetWorldScale();
                Vector3 rot = mfObj->GetModel()->GetWorldRotate();

                if(ImGui::DragFloat3("Position", &pos.x, 0.1f)){
                    mfObj->SetTranslate(pos);
                }
                if(ImGui::DragFloat3("Scale", &scl.x, 0.1f)){
                    mfObj->SetScale(scl);
                }
                if(ImGui::DragFloat3("Rotation", &rot.x, 0.01f)){
                    mfObj->SetRotate(rot);
                }

                // 削除ボタン
                ImGui::Separator();
                if(ImGui::Button("Remove Selected Model")){
                    objects.erase(objects.begin() + selectedObjIndex);
                    selectedObjIndex = -1;
                }
            }
        } else{
            ImGui::Text("No Model Selected");
        }
    }

    ImGui::EndChild();

    ImGui::End();
#endif // _DEBUG
}
