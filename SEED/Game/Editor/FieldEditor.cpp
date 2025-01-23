#include "FieldEditor.h"

// local
#include "FieldObject/Door/FieldObject_Door.h"
#include "FieldObject/GrassSoil/FieldObject_GrassSoil.h"
#include "FieldObject/Soil/FieldObject_Soil.h"
#include "FieldObject/Sphere/FieldObject_Sphere.h"
#include "FieldObject/Start/FieldObject_Start.h"
#include "FieldObject/Goal/FieldObject_Goal.h"
#include "FieldObject/Switch/FieldObject_Switch.h"
#include "FieldObject/ViewPoint/FieldObject_ViewPoint.h"

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
FieldEditor::FieldEditor(StageManager& manager)
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
    modelNameMap_["viewpoint"] = FIELDMODEL_VIEWPOINT;

    LoadFieldModelTexture();

    edittingStageIndex = StageManager::GetCurrentStageNo();
}


////////////////////////////////////////////////////////////////////////////////////////
//  全てのスイッチから特定のドア参照を削除する関数
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::RemoveDoorFromAllSwitches(FieldObject_Door* doorToRemove, const std::vector<FieldObject_Switch*>& allSwitches) const{
    for(auto* sw : allSwitches){
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
//  jsonファイルの名前を入力するフィールド
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::PopupDecideOutputName(){

    // ファイル名入力
    std::string text = "Output stage_" + std::to_string(edittingStageIndex + 1) + ".json?";
    ImGui::Text(text.c_str());

    // OKボタン
    if(ImGui::Button("OK", ImVec2(120, 0))){
        std::string fileName = jsonPath_ + "stage_" + std::to_string(edittingStageIndex + 1);
        // ファイル名に拡張子がない場合、追加
        if(fileName.find(".json") == std::string::npos){
            fileName += ".json";
        }

        // もしファイルが存在していたら上書き確認
        if(std::filesystem::exists(fileName)){
            ImGui::OpenPopup("Overwrite?");
        } else{
            SaveToJson(fileName, edittingStageIndex + 1);

            // 成功ログの表示
            MessageBoxA(nullptr, "Json is Saved", "SaveToJson", MB_OK);

            ImGui::CloseCurrentPopup();
        }
    }

    // キャンセルボタン
    ImGui::SameLine();
    if(ImGui::Button("Cancel", ImVec2(120, 0))){
        ImGui::CloseCurrentPopup();
    }

    // 上書き確認ポップアップ
    if(ImGui::BeginPopupModal("Overwrite?", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
        ImGui::Text("The file already exists.");
        ImGui::Text("Do you want to overwrite it?");

        if(ImGui::Button("OK", ImVec2(120, 0))){
            std::string fileName = jsonPath_ + "stage_" + std::to_string(edittingStageIndex + 1);
            if(fileName.find(".json") == std::string::npos){
                fileName += ".json";
            }
            SaveToJson(fileName, edittingStageIndex + 1);
            // 成功ログの表示
            MessageBoxA(nullptr, "Json is Saved", "SaveToJson", MB_OK);

            ImGui::CloseCurrentPopup();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if(ImGui::Button("Cancel", ImVec2(120, 0))){
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//  jsonファイルの保存
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::SaveToJson(const std::string& filePath, int32_t stageNo){
    try{
        namespace fs = std::filesystem;
        fs::path path(filePath);
        auto directory = path.parent_path();

        if(!directory.empty() && !fs::exists(directory)){
            fs::create_directories(directory);
        }

        nlohmann::json jsonData;
        jsonData["stage"] = stageNo;

        // Managerからオブジェクトを取得
        auto& objects = manager_.GetStages()[stageNo - 1]->GetObjects();
        for(size_t i = 0; i < objects.size(); ++i){
            // dynamic_cast で ModelFieldObject のみ処理
            auto* modelObj = dynamic_cast<FieldObject*>(objects[i].get());
            if(!modelObj) continue;

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

            // スイッチの場合、関連付けられたドアのIDを保存
            if(auto* sw = dynamic_cast<FieldObject_Switch*>(modelObj)){
                std::vector<int> doorIDs;
                for(auto* door : sw->GetAssociatedDoors()){
                    doorIDs.push_back(door->GetFieldObjectID());
                }
                modelJson["associatedDoors"] = doorIDs;
            }

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
    putPos.y -= kBlockSize * Input::GetMouseWheel();

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
        auto& stage = manager_.GetStages()[edittingStageIndex];
        stage->AddModel(objectType, { kBlockScale, kBlockScale, kBlockScale }, { 0.0f,0.0f,0.0f }, putPos);
        isEdit = false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////
//  各fieldObjectのID再割り当て
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::ReassignIDsByType(uint32_t removedType, std::vector<std::unique_ptr<FieldObject>>& objects){
    switch(removedType){
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
    case FIELDMODELNAME::FIELDMODEL_VIEWPOINT:
        ReassignIDsForType<FieldObject_ViewPoint>(objects);
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
    static FieldObject_Switch* assigningSwitch = nullptr;


    edittingStageIndex = StageManager::GetCurrentStageNo();

    ImGui::Begin("Field Editor");

    //----------------------------------------
    // [1] ステージ設定, セーブなど
    //----------------------------------------
    {
        ImGui::Text("Stage Settings");
        ImGui::Separator();

        ImGui::Checkbox("isEditing", &isEditing_);

        ImGui::SameLine();
        ImGui::Text("Stage:");
        ImGui::SameLine();
        if(ImGui::Combo("##stageNo##1", &edittingStageIndex,
            "1\0"
            "2\0"
            "3\0"
            "4\0"
            "5\0"
            "6\0"
            "7\0"
            "8\0"
            "9\0"
            "10\0\0"
        )){
            manager_.SetCurrentStageNo(edittingStageIndex);
        }

        // モード切り替え用 ラジオボタン
        ImGui::Text("Editor Mode:");
        ImGui::SameLine();
        if(ImGui::RadioButton("Add FieldObject", editorMode_ == EditorMode::AddFieldObject)){
            editorMode_ = EditorMode::AddFieldObject;
        }
        ImGui::SameLine();
        if(ImGui::RadioButton("Add Enemy", editorMode_ == EditorMode::AddEnemy)){
            editorMode_ = EditorMode::AddEnemy;
        }

        ImGui::SameLine();

        if(ImGui::Button("Output StageData")){
            ImGui::OpenPopup("Output to Json");
        }
        if(ImGui::BeginPopupModal("Output to Json", NULL, ImGuiWindowFlags_AlwaysAutoResize)){
            PopupDecideOutputName();
            ImGui::EndPopup();
        }
    }

    ImGui::Separator();
    ImGui::Spacing();

    //----------------------------------------
    // [2] モードに応じた処理分岐
    //----------------------------------------

    switch(editorMode_){
    case EditorMode::AddFieldObject:
        // 既存のフィールドオブジェクト追加処理
        AddObjectByMouse(selectItemIdxOnGUI_);
        break;
    case EditorMode::AddEnemy:
        // 敵配置処理を呼び出す
        AddEnemyByMouse();
        break;
    default:
        break;
    }

    //----------------------------------------
    // [3] フィールドオブジェクト 追加パネル
    //----------------------------------------
    {
        ImGui::Text("Add FieldObject");
        ImGui::Separator();

        ImGui::Text("Select a Model to Add:");
        static std::string selectedModelName = "GrassSoil";
        int i = 0;
        for(auto& map : modelNameMap_){
            std::string imageKey = "fieldModelTextures/" + map.first + "Image.png";
            auto it = textureIDs_.find(imageKey);

            // ボタンの表示(追加するアイテムインデックスを選択)
            if(it != textureIDs_.end()){
                if(ImGui::ImageButton(it->second, ImVec2(64, 64))){
                    selectItemIdxOnGUI_ = map.second;
                    selectedModelName = map.first;
                }
            } else{
                if(ImGui::Button(map.first.c_str(), ImVec2(64, 64))){
                    selectItemIdxOnGUI_ = map.second;
                    selectedModelName = map.first;
                }
            }

            if(i % 6 != 5){ ImGui::SameLine(); }


            i++;
        }

        ImGui::NewLine();

        // 選択しているアイテム名の表示
        ImGui::Text("Selected: %s", selectedModelName.c_str());

    }

    ImGui::Separator();
    ImGui::Spacing();

    //----------------------------------------
    // [3] オブジェクト一覧(左) / 編集パネル(右) の2分割
    //----------------------------------------
    auto& objects = manager_.GetStages()[edittingStageIndex]->GetObjects();
    static int selectedObjIndex = -1;

    // 2カラムレイアウト開始
    ImGui::Columns(2, "2cols", true);

    //-------------------------
    // 3-A) 左: オブジェクト一覧
    //-------------------------
    ImGui::BeginChild("ModelList", ImVec2(0, 0), true);
    {
        ImGui::Text("Model List:");
        ImGui::Separator();

        for(int idx = 0; idx < (int)objects.size(); idx++){
            auto* mfObj = dynamic_cast<FieldObject*>(objects[idx].get());
            if(!mfObj) continue;
            int id = mfObj->GetFieldObjectID();

            // リスト表示用ラベル
            std::string label = mfObj->GetName() + std::to_string(id);
            bool isSelected = (selectedObjIndex == idx);

            // 選択されている場合は色を変える
            if(isSelected){
                mfObj->SetColor({ 1.f, 0.f, 0.f, 1.f });
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.f));
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.f, 1.f));
                if(ImGui::Selectable(label.c_str(), true)){
                    // 特に何もせず
                }
                ImGui::PopStyleColor(2);
            } else{
                mfObj->SetColor({ 1.f, 1.f, 1.f, 1.f });
                if(ImGui::Selectable(label.c_str(), false)){
                    selectedObjIndex = idx;
                }
            }
        }

        // 左クリックで直接オブジェクトを選択
        if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
            // ImGuiウィンドウ上をクリックした場合を除外
            if(!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)){
                selectedObjIndex = GetObjectIndexByMouse(objects);
                
            }
        }
    }
    ImGui::EndChild();

    // 次のカラムへ
    ImGui::NextColumn();

    //-------------------------
    // 3-B) 右: 選択中オブジェクトの編集パネル
    //-------------------------
    ImGui::BeginChild("ModelEditor", ImVec2(0, 0), true);
    {
        if(selectedObjIndex >= 0 && selectedObjIndex < (int)objects.size()){
            auto* mfObj = dynamic_cast<FieldObject*>(objects[selectedObjIndex].get());
            if(mfObj){
                ImGui::Text("Editing Model (Index=%d)", selectedObjIndex);
                ImGui::Separator();

                //選択されているオブジェクトを設定
                Stage* stage = manager_.GetStages()[edittingStageIndex].get();
                stage->SetSelectedObject(mfObj);

                // [A] スイッチの場合の設定
                if(auto* sw = dynamic_cast<FieldObject_Switch*>(mfObj)){
                    ImGui::Text("Switch Settings");
                    ImGui::Separator();
                    // 「ドア割り当て」モード開始
                    assigningSwitch = sw;

                    // すでに関連付けられているドアを黄色にする
                    for(auto* door : sw->GetAssociatedDoors()){
                        door->SetColor({ 1.f, 1.f, 0.f, 1.f });
                    }
                }

                // [B] チャンク移動/スケール
                {
                    static Vector3Int moveChunk{ 0, 0, 0 };
                    static Vector3Int stageMoveChunk{ 0, 0, 0 };
                    static int lastSelectedIndex = -1;

                    // オブジェクトが変わったら値を初期化
                    if(selectedObjIndex != lastSelectedIndex){
                        moveChunk = { 0, 0, 0 };
                        stageMoveChunk = { 0, 0, 0 };
                        lastSelectedIndex = selectedObjIndex;
                    }

                    ImGui::Text("Chunk Transform");
                    ImGui::Separator();
                    Vector3Int tempMove = moveChunk;
                    Vector3Int tempStageMove = stageMoveChunk;
                    ImGui::DragInt3("StageMove", &tempStageMove.x, 0.1f);
                    ImGui::DragInt3("Move (chunks)", &tempMove.x, 0.1f);

                    Vector3 pos = mfObj->GetModel()->GetWorldTranslate();
                    Vector3 scl = mfObj->GetModel()->GetWorldScale();

                    // ステージ全体移動
                    if(tempStageMove != stageMoveChunk){
                        Vector3 diff{
                            float(tempStageMove.x - stageMoveChunk.x) * kBlockSize,
                            float(tempStageMove.y - stageMoveChunk.y) * kBlockSize,
                            float(tempStageMove.z - stageMoveChunk.z) * kBlockSize
                        };
                        // 全オブジェクトを移動
                        for(auto& obj : objects){
                            obj->AddTranslate(diff);
                        }
                        stageMoveChunk = tempStageMove;
                    }

                    // 単体移動
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
                }

                // [C] Position / Scale / Rotation の個別編集
                {
                    Vector3 pos = mfObj->GetModel()->GetWorldTranslate();
                    Vector3 scl = mfObj->GetModel()->GetWorldScale();
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
                }

                ImGui::Separator();

                // [D] オブジェクト削除
                if(ImGui::Button("Remove Selected Model")||Input::IsTriggerKey(DIK_ESCAPE)){
                    if(selectedObjIndex >= 0 && selectedObjIndex < (int)objects.size()){
                        FieldObject* objToRemove = objects[selectedObjIndex].get();

                        // スイッチに紐づいたドアを外す
                        std::vector<FieldObject_Switch*> allSwitches =
                            manager_.GetStages()[edittingStageIndex]->GetObjectsOfType<FieldObject_Switch>();

                        uint32_t removedType = objToRemove->GetFieldObjectType();

                        // ドア型なら、関連スイッチから削除
                        if(auto* door = dynamic_cast<FieldObject_Door*>(objToRemove)){
                            RemoveDoorFromAllSwitches(door, allSwitches);
                        }

                        // Manager 側でオブジェクト削除
                        manager_.GetStages()[edittingStageIndex]->RemoveFieldObject(objToRemove);

                        // ID 再割り当て
                        ReassignIDsByType(removedType, objects);

                        selectedObjIndex = -1;
                    }
                }
            } else{
                ImGui::Text("No Model Selected? (null object)");
            }
        } else{
            ImGui::Text("No Model Selected");
        }
    }
    ImGui::EndChild();

    // カラム終了
    ImGui::Columns(1);

    //----------------------------------------
    // [4] 中ボタンクリックでスイッチとドアを関連付け/解除
    //----------------------------------------
    if(Input::IsTriggerMouse(MOUSE_BUTTON::MIDDLE)){
        int clickedIndex = GetObjectIndexByMouse(objects);
        if(clickedIndex >= 0){
            auto* clickedObj = dynamic_cast<FieldObject*>(objects[clickedIndex].get());
            if(auto* door = dynamic_cast<FieldObject_Door*>(clickedObj)){
                // ドアが既にスイッチを持っている場合 = 解除
                if(door->GetHasSwitch()){
                    assigningSwitch->RemoveAssociatedDoor(door);
                    door->RemoveSwitch(assigningSwitch);
                }
                // 未設定なら紐付け
                else if(assigningSwitch){
                    assigningSwitch->AddAssociatedDoor(door);
                    door->SetSwitch(assigningSwitch);
                    assigningSwitch = nullptr;
                }
            }
        }
    }

    // ウィンドウ終了
    ImGui::End();

#endif // _DEBUG
}



////////////////////////////////////////////////////////////////////////////////////////
//  敵を配置するフロー用
////////////////////////////////////////////////////////////////////////////////////////
void FieldEditor::AddEnemyByMouse(){
    static Vector3 putPos = { 0, 0, 0 };
    static bool isPlacing = false;

    // 右クリックが押された瞬間に「配置開始」
    if(Input::IsTriggerMouse(MOUSE_BUTTON::RIGHT)){
        isPlacing = true;
    }
    if(!isPlacing){ return; }

    // マウスホイールで高さ(Y座標)を変えるなど
    putPos.y -= kBlockSize * Input::GetMouseWheel();

    // y = putPos.y の平面との交点をとる
    Quad plane;
    plane.localVertex[0] = { -1000.0f, putPos.y,  1000.0f };
    plane.localVertex[1] = { 1000.0f, putPos.y,  1000.0f };
    plane.localVertex[2] = { -1000.0f, putPos.y, -1000.0f };
    plane.localVertex[3] = { 1000.0f, putPos.y, -1000.0f };

    // レイを取得
    Line ray = SEED::GetCamera()->GetRay(Input::GetMousePosition());
    CollisionData data = Collision::Quad::Line(plane, ray);
    if(data.hitPos.has_value()){
        Vector3 hitPos = data.hitPos.value();
        putPos.x = kBlockSize * int(hitPos.x / kBlockSize);
        putPos.z = kBlockSize * int(hitPos.z / kBlockSize);
    }

    // AABBでガイドを描くなど
    AABB aabb;
    aabb.center = putPos + Vector3(0.0f, kBlockSize * 0.5f, 0.0f);
    aabb.halfSize = Vector3(kBlockSize * 0.5f, kBlockSize * 0.5f, kBlockSize * 0.5f);
    SEED::DrawAABB(aabb, { 1.f, 0.f, 0.f, 1.f }); // デバッグ描画(任意)

    // 右クリックが離されたら確定
    if(Input::IsReleaseMouse(MOUSE_BUTTON::RIGHT)){
        // 現在ステージの EnemyManager を取得
        auto* currentStage = manager_.GetStages()[edittingStageIndex].get();
        auto* enemyManager = currentStage->GetEnemyManager();
        if(enemyManager){
            // 新規敵を追加
            const std::string newEnemyName = "enemy" + std::to_string(enemyManager->GetEnemies().size());
           std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>(enemyManager, enemyManager->GetPlayer(), newEnemyName);
            newEnemy->SetPosition(putPos);
            enemyManager->AddEnemy(std::move(newEnemy));
        }
        isPlacing = false;
    }
}

int32_t FieldEditor::GetEnemyIndexByMouse(const std::vector<std::unique_ptr<Enemy>>& enemies){
    Line ray = SEED::GetCamera()->GetRay(Input::GetMousePosition());
    float minDist = FLT_MAX;
    int32_t index = -1;

    for(int32_t i = 0; i < (int32_t)enemies.size(); i++){
        Enemy* e = enemies[i].get();
        if(!e) continue;

        // 敵のコリジョン判定があるなら同様に
        // ここでは簡易的に「敵の当たり判定用AABB or Sphere」を持っている想定
        auto& colliders = e->GetColliders();
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

void FieldEditor::UpdateSelectionByMouse(){
    // 左クリックで選択処理
    if(!Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
        return;
    }
    if(ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)){
        // もしUI上のボタンをクリックしたなら、3D選択処理はしない
        return;
    }

    // 1) FieldObject の最も近いものを探す
    auto& objects = manager_.GetStages()[edittingStageIndex]->GetObjects();
    int objIdx = GetObjectIndexByMouse(objects);
    float distObj = -1.f;
    if(objIdx >= 0){
        FieldObject* obj = objects[objIdx].get();
        // 何らかの方法でフィールドオブジェクトまでの距離を出す
        distObj = MyMath::Length(obj->GetModel()->GetWorldTranslate() - SEED::GetCamera()->GetTranslation());
    }

    // 2) Enemy の最も近いものを探す
    auto* stage = manager_.GetStages()[edittingStageIndex].get();
    auto* em = stage->GetEnemyManager();
    float distEnemy = -1.f;
    int eneIdx = -1;
    if(em){
        eneIdx = GetEnemyIndexByMouse(em->GetEnemies());
        if(eneIdx >= 0){
            auto& e = em->GetEnemies()[eneIdx];
            distEnemy = MyMath::Length(e->GetWorldTranslate() - SEED::GetCamera()->GetTranslation());
        }
    }

    // 3) どちらも -1 (未ヒット) なら選択解除
    if(objIdx < 0 && eneIdx < 0){
        selectedObjIndex_ = -1;
        selectedEnemyIndex_ = -1;
        selectedIsEnemy_ = false;
        return;
    }

    // 4) どっちが近いか比較
    if(objIdx >= 0 && eneIdx >= 0){
        if(distObj < distEnemy){
            // FieldObject が近い
            selectedObjIndex_ = objIdx;
            selectedIsEnemy_ = false;
            selectedEnemyIndex_ = -1;
        } else{
            // Enemy が近い
            selectedEnemyIndex_ = eneIdx;
            selectedIsEnemy_ = true;
            selectedObjIndex_ = -1;
        }
    } else if(objIdx >= 0){
        // 敵はヒットせず FieldObject だけヒット
        selectedObjIndex_ = objIdx;
        selectedIsEnemy_ = false;
        selectedEnemyIndex_ = -1;
    } else{
        // フィールドオブジェクトはヒットせず Enemyだけヒット
        selectedEnemyIndex_ = eneIdx;
        selectedIsEnemy_ = true;
        selectedObjIndex_ = -1;
    }
}


