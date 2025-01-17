#include "FieldEditor.h"

// local
#include "FieldObject/Door/FieldObject_Door.h"
#include "FieldObject/GrassSoil/FieldObject_GrassSoil.h"
#include "FieldObject/Soil/FieldObject_Soil.h"
#include "FieldObject/Sphere/FieldObject_Sphere.h"
#include "FieldObject/Start/FieldObject_Start.h"
#include "FieldObject/Goal/FieldObject_Goal.h"
#include "FieldObject/Switch/FieldObject_Switch.h"

//engine
#include "../SEED/external/imgui/imgui.h"
#include "../SEED.h"
#include "CollisionManaer/Collision.h"

//lib
#include "../adapter/json/JsonCoordinator.h"
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
    modelNameMap_["switch"] = FIELDMODEL_SWITCH;

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
    if (modelNameIndex == FIELDMODEL_START || modelNameIndex == FIELDMODEL_GOAL){
        auto& objects = manager_.GetObjects();
        for (const auto& objPtr : objects){
            FieldObject* obj = objPtr.get();
            if (obj && obj->GetFieldObjectType() == modelNameIndex){
                // 既に同じタイプのオブジェクトが存在する場合、追加をキャンセル
                return;
            }
        }
    }

    // 新規オブジェクトの生成
    std::unique_ptr<FieldObject> newObj = nullptr;

    switch (modelNameIndex){
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
        case FIELDMODEL_SWITCH:
            newObj = std::make_unique<FieldObject_Switch>();
            break;
        default:
            break;
    }

    if (!newObj) return;  // newObj が生成されなかった場合は何もしない

    // スタートまたはゴールの場合、スケールを1/10に調整
    Vector3 adjustedScale = scale;
    if (modelNameIndex == FIELDMODEL_START || modelNameIndex == FIELDMODEL_GOAL){
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


void FieldEditor::RemoveDoorFromAllSwitches(FieldObject_Door* doorToRemove, const std::vector<FieldObject_Switch*>& allSwitches) const{
    for (auto* sw : allSwitches){
        auto& associatedDoors = sw->GetAssociatedDoors();
        auto& observers = sw->GetObservers();

        // 関連ドアから削除
        observers.erase(
            std::remove(observers.begin(), observers.end(), doorToRemove),
            observers.end()
        );

        associatedDoors.erase(
            std::remove(associatedDoors.begin(), associatedDoors.end(), doorToRemove),
            associatedDoors.end()
        );
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//  jsonファイルの読み込み
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::LoadFromJson(const std::string& filePath){
    namespace fs = std::filesystem;

    if (!fs::exists(filePath)){
        return;
    }
    std::ifstream file(filePath);
    if (!file.is_open()){
        return;
    }

    nlohmann::json jsonData;
    file >> jsonData;
    file.close();

    // いったん全消去
    manager_.ClearAllFieldObjects();

    // スイッチと関連ドアIDの一時保存用
    std::vector<std::tuple<FieldObject_Switch*, std::vector<int>>> switchDoorAssociations;

    // JSON から "models" 配列を読み取り
    if (jsonData.contains("models")){
        for (auto& modelJson : jsonData["models"]){
            std::string name = modelJson.value("name", "default_model.obj");
            uint32_t type = 0;
            Vector3 position {0.f, 0.f, 0.f};
            Vector3 scale {1.f, 1.f, 1.f};
            Vector3 rotation {0.f, 0.f, 0.f};

            if (modelJson.contains("position")){
                position.x = modelJson["position"][0];
                position.y = modelJson["position"][1];
                position.z = modelJson["position"][2];
            }
            if (modelJson.contains("scale")){
                scale.x = modelJson["scale"][0];
                scale.y = modelJson["scale"][1];
                scale.z = modelJson["scale"][2];
            }
            if (modelJson.contains("rotation")){
                rotation.x = modelJson["rotation"][0];
                rotation.y = modelJson["rotation"][1];
                rotation.z = modelJson["rotation"][2];
            }
            if (modelJson.contains("type")){
                type = modelJson["type"];
            }

            // 取得した情報からモデルを追加
            AddModel(type, scale, rotation, position);

            // 新規追加されたオブジェクトを取得（最後に追加されたものを想定）
            auto& objects = manager_.GetObjects();
            if (objects.empty()) continue;
            FieldObject* newObj = objects.back().get();

            // スイッチの場合、関連ドア情報を一時保存
            if (auto* sw = dynamic_cast< FieldObject_Switch* >(newObj)){
                //json から associatedDoors を取得(ドアIDの配列)
                if (modelJson.contains("associatedDoors")){
                    std::vector<int> doorIDs = modelJson["associatedDoors"].get<std::vector<int>>();
                    switchDoorAssociations.emplace_back(sw, doorIDs);
                }
            }
        }
    }

    // 全てのモデルを生成・登録後に、スイッチとドアの関連付けを行う
    for (auto& [sw, doorIDs] : switchDoorAssociations){
        for (uint32_t doorID : doorIDs){
            // manager_ から対応するドアを検索
            std::vector<FieldObject_Door*> doors = manager_.GetObjectsOfType<FieldObject_Door>();
            for (auto& door : doors){
                if (door){
                    if (door->GetFieldObjectID() == doorID){
                        // スイッチにドアを追加し、ドア側でスイッチをセット
                        sw->AddAssociatedDoor(door);
                        door->SetSwitch(sw);
                        // 1つのドアにつき1回で十分と仮定
                        break;
                    }
                }
            }
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

        if (!directory.empty() && !fs::exists(directory)){
            fs::create_directories(directory);
        }

        nlohmann::json jsonData;

        // Managerからオブジェクトを取得
        auto& objects = manager_.GetObjects();
        for (size_t i = 0; i < objects.size(); ++i){
            auto* modelObj = dynamic_cast< FieldObject* >(objects[i].get());
            if (!modelObj) continue;

            std::string modelName = modelObj->GetName();
            const std::string prefix = "assets/";
            if (modelName.rfind(prefix, 0) == 0){
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

            // スイッチの場合、関連付けられたドアのIDを保存
            if (auto* sw = dynamic_cast< FieldObject_Switch* >(modelObj)){
                std::vector<int> doorIDs;
                for (auto* door : sw->GetAssociatedDoors()){
                    doorIDs.push_back(door->GetFieldObjectID());
                }
                modelJson["associatedDoors"] = doorIDs;
            }

            jsonData["models"].push_back(modelJson);
        }

        // JSONファイルに書き込み
        std::ofstream outFile(filePath);
        if (outFile.is_open()){
            outFile << jsonData.dump(4);
            outFile.close();
        }
    } catch ([[maybe_unused]] const std::exception& e){
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
    for (int32_t i = 0; i < objects.size(); ++i){
        auto* obj = objects[i].get();
        if (!obj) continue;

        // オブジェクトのコライダーを取得
        auto& colliders = obj->GetColliders();
        for (auto& collider : colliders){
            // Rayと当たり判定
            if (collider->CheckCollision(ray)){

                // 距離を取得
                float dist = MyMath::Length(collider->GetWoarldTranslate() - ray.origin_);

                // もし距離が一番近い場合、そのオブジェクトを選択
                if (dist < minDist){
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
    static Vector3 putPos = {0.0f,0.0f,0.0f};
    static bool isEdit = false;

    // 基準位置やoffsetをリセット
    if (Input::IsTriggerMouse(MOUSE_BUTTON::RIGHT)){
        isEdit = true;
    }

    // 編集中でない場合は何もしない
    if (!isEdit){ return; }

    // y座標を更新
    putPos.y += kBlockSize * Input::GetMouseWheel();

    // y軸の高さのxz平面を求める
    Quad plane;
    plane.localVertex[0] = {-1000.0f,putPos.y, 1000.0f,};
    plane.localVertex[1] = {1000.0f,putPos.y, 1000.0f,};
    plane.localVertex[2] = {-1000.0f,putPos.y,-1000.0f,};
    plane.localVertex[3] = {1000.0f,putPos.y,-1000.0f,};

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
    SEED::DrawAABB(aabb, {1.0f,1.0f,0.0f,1.0f});

    // マウスのボタンが離されたらオブジェクトを追加
    if (Input::IsReleaseMouse(MOUSE_BUTTON::RIGHT)){
        AddModel(objectType, {kBlockScale, kBlockScale, kBlockScale}, {0.0f,0.0f,0.0f}, putPos);
        isEdit = false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//  各fieldObjectのID再割り当て
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::ReassignIDsByType(uint32_t removedType, std::vector<std::unique_ptr<FieldObject>>& objects){
    switch (removedType){
        case FIELDMODELNAME::FIELDMODEL_GRASSSOIL:
            ReassignIDsForType<FieldObject_GrassSoil>(objects);
            break;
        case FIELDMODELNAME::FIELDMODEL_SOIL:
            ReassignIDsForType<FieldObject_Soil>(objects);
            break;
        case FIELDMODELNAME::FIELDMODEL_SPHERE:
            ReassignIDsForType<FieldObject_Sphere>(objects);
            break;
        case FIELDMODELNAME::FIELDMODEL_DOOR:
            ReassignIDsForType<FieldObject_Door>(objects);
            break;
        case FIELDMODELNAME::FIELDMODEL_START:
            ReassignIDsForType<FieldObject_Start>(objects);
            break;
        case FIELDMODELNAME::FIELDMODEL_GOAL:
            ReassignIDsForType<FieldObject_Goal>(objects);
            break;
        case FIELDMODELNAME::FIELDMODEL_SWITCH:
            ReassignIDsForType<FieldObject_Switch>(objects);
            break;
        default:
            // 必要に応じてデフォルト処理を追加
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//  imguiの表示
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::ShowImGui(){
#ifdef _DEBUG
    // 紐付けモード用の静的変数を追加
    static bool isAssigningDoor = false;
    static FieldObject_Switch* assigningSwitch = nullptr;

    ImGui::Begin("Field Editor");

    ImGui::Checkbox("isEditing", &isEditing_);
    ImGui::Text("mouseWheel: %d", Input::GetMouseWheel());

    // 選択されているモデルのインデックス
    static int selectedModelIndex = -1;

    // ========== モデルサムネの一覧をボタンで並べる ==========
    ImGui::Text("Select a Model to Add:");
    int i = 0;
    for (auto& map : modelNameMap_){
        std::string imageKey = "fieldModelTextures/" + map.first + "Image.png";
        auto it = textureIDs_.find(imageKey);

        if (it != textureIDs_.end()){
            if (ImGui::ImageButton(it->second, ImVec2(64, 64))){
                AddModel(map.second);
            }
        } else{
            if (ImGui::Button(map.first.c_str(), ImVec2(64, 64))){
                AddModel(map.second);
            }
        }
        ImGui::SameLine();
        i++;
    }
    ImGui::NewLine();

    AddObjectByMouse(FIELDMODEL_GRASSSOIL);

    if (ImGui::Button("Save Models")){
        SaveToJson(jsonPath_);
    }

    ImGui::Separator();

    // Managerから取得
    auto& objects = manager_.GetObjects();

    static int selectedObjIndex = -1;
    ImGui::BeginChild("ModelList", ImVec2(200, 300), true);
    {
        ImGui::Text("Model List:");
        for (int idx = 0; idx < ( int ) objects.size(); idx++){
            auto* mfObj = dynamic_cast< FieldObject* >(objects[idx].get());
            if (!mfObj) continue;
            int id = mfObj->GetFieldObjectID();
            std::string label = mfObj->GetName() + std::to_string(id);
            bool isSelected = (selectedObjIndex == idx);

            if (isSelected){
                mfObj->SetColor({1.f, 0.f, 0.f, 1.f});
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.f));
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.f, 1.f));
                ImGui::Selectable(label.c_str(), true);
                ImGui::PopStyleColor(2);
            } else{
                mfObj->SetColor({1.f, 1.f, 1.f, 1.f});
                if (ImGui::Selectable(label.c_str(), false)){
                    selectedObjIndex = idx;
                }
            }
        }
    }
    ImGui::EndChild();

    if (Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
        if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)){
            selectedObjIndex = GetObjectIndexByMouse(objects);
        }
    }

    ImGui::SameLine();

    ImGui::BeginChild("ModelEditor", ImVec2(250, 300), true);
    {
        if (selectedObjIndex >= 0 && selectedObjIndex < ( int ) objects.size()){
            auto* mfObj = dynamic_cast< FieldObject* >(objects[selectedObjIndex].get());
            if (mfObj){
                ImGui::Text("Editing Model %d", selectedObjIndex);
                ImGui::Separator();

                // --- スイッチ設定セクション ---
                if (auto* sw = dynamic_cast< FieldObject_Switch* >(mfObj)){
                    ImGui::Separator();
                    ImGui::Text("Switch Settings");

                    // 「Assign Door」ボタンを追加
                    if (ImGui::Button("Assign Door")){
                        isAssigningDoor = true;
                        assigningSwitch = sw;
                    }

                    // 選択されているスイッチに関連付けられた全てのドアを黄色に設定
                    for (auto* door : sw->GetAssociatedDoors()){
                        door->SetColor({1.f, 1.f, 0.f, 1.f}); // 黄色
                    }
                }

                // --- 1) チャンク移動/スケール用 変数 ---
                static Vector3Int moveChunk {0, 0, 0};
                static Vector3Int scaleChunk {1, 1, 1};
                static int lastSelectedIndex = -1;

                if (selectedObjIndex != lastSelectedIndex){
                    Vector3 curScale = mfObj->GetModel()->GetWorldScale();
                    scaleChunk.x = ( int ) (curScale.x / 10.f);
                    scaleChunk.y = ( int ) (curScale.y / 10.f);
                    scaleChunk.z = ( int ) (curScale.z / 10.f);

                    moveChunk = {0, 0, 0};
                    lastSelectedIndex = selectedObjIndex;
                }

                ImGui::Text("Chunk Transform");
                Vector3Int tempMove = moveChunk;
                ImGui::DragInt3("Move (chunks)", &tempMove.x, 0.1f);

                Vector3 pos = mfObj->GetModel()->GetWorldTranslate();
                Vector3 scl = mfObj->GetModel()->GetWorldScale();

                if (tempMove != moveChunk){
                    Vector3Int diff {
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

                pos = mfObj->GetModel()->GetWorldTranslate();
                scl = mfObj->GetModel()->GetWorldScale();
                Vector3 rot = mfObj->GetModel()->GetWorldRotate();

                if (ImGui::DragFloat3("Position", &pos.x, 0.1f)){
                    mfObj->SetTranslate(pos);
                }
                if (ImGui::DragFloat3("Scale", &scl.x, 0.1f)){
                    mfObj->SetScale(scl);
                }
                if (ImGui::DragFloat3("Rotation", &rot.x, 0.01f)){
                    mfObj->SetRotate(rot);
                }

                ImGui::Separator();
                if (ImGui::Button("Remove Selected Model")){
                    if (selectedObjIndex >= 0 && selectedObjIndex < objects.size()){
                        // 削除対象オブジェクトの取得
                        FieldObject* objToRemove = objects[selectedObjIndex].get();

                        std::vector<FieldObject_Switch*> allSwitches = manager_.GetObjectsOfType<FieldObject_Switch>();

                        // 削除対象オブジェクトのタイプを取得
                        uint32_t removedType = objToRemove->GetFieldObjectType();

                        // ドア型かどうかを確認
                        if (auto* door = dynamic_cast< FieldObject_Door* >(objToRemove)){
                            // door を全スイッチから削除
                            RemoveDoorFromAllSwitches(door, allSwitches);
                        }

                        // Manager 側でオブジェクト削除
                        manager_.RemoveFieldObject(objToRemove);

                        selectedObjIndex = -1;

                        // ID再割り当て
                        ReassignIDsByType(removedType, objects);

                    }
                }
            } else{
                ImGui::Text("No Model Selected");
            }
        }
        ImGui::EndChild();

        // 紐付けモード中にマウスクリックでドアを選択
        if (isAssigningDoor && Input::IsTriggerMouse(MOUSE_BUTTON::MIDDLE)){
            int clickedIndex = GetObjectIndexByMouse(objects);
            if (clickedIndex >= 0){
                auto* clickedObj = dynamic_cast< FieldObject* >(objects[clickedIndex].get());
                if (auto* door = dynamic_cast< FieldObject_Door* >(clickedObj)){
                    if (assigningSwitch){

                        assigningSwitch->AddAssociatedDoor(door);

                        // ドア側でスイッチをセットし、オブザーバー登録を行う
                        door->SetSwitch(assigningSwitch);
                    }
                    isAssigningDoor = false;
                    assigningSwitch = nullptr;
                }
            }
        }

        ImGui::End();
    #endif // _DEBUG
    }
}