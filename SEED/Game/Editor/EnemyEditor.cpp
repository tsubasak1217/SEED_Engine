#include "EnemyEditor.h"

// local
#include "../Manager/EnemyManager.h"
#include "../Manager/StageManager.h"
#include "../SEED.h"

// lib
#include "../adapter/json/JsonCoordinator.h"
#include "../adapter/csv/CsvAdapter.h"
#include "imgui.h"

EnemyEditor::EnemyEditor(){
}

void EnemyEditor::ShowImGui(){
#ifdef _DEBUG
    //---------------------------------------------------------------------------------
    // 1) 現在ステージと EnemyManager を取得
    //---------------------------------------------------------------------------------
    Stage* currentStage = StageManager::GetCurrentStage();
    if (!currentStage){
        ImGui::Begin("Enemy Editor");
        ImGui::Text("No current stage.");
        ImGui::End();
        return;
    }
    EnemyManager* em = StageManager::GetCurrentStage()->GetEnemyManager();
    if (!em){
        ImGui::Begin("Enemy Editor");
        ImGui::Text("No EnemyManager in current stage.");
        ImGui::End();
        return;
    }

    //---------------------------------------------------------------------------------
    // 2) UI表示開始
    //---------------------------------------------------------------------------------
    ImGui::Begin("Enemy Editor");

    // ---- Add Enemy, Save ----
    ImGui::SameLine();
    if (ImGui::Button("Save Enemies")){
        // JSON/CSV にセーブ
        SaveEnemies();
    }

    ImGui::Separator();

    //---------------------------------------------------------------------------------
    // 3) 左側のエネミー一覧
    //---------------------------------------------------------------------------------
    ImGui::BeginChild("Enemy List", ImVec2(200, 300), true);
    {
        ImGui::Text("Enemies:");
        auto& enemies = em->GetEnemies();
        for (uint32_t i = 0; i < enemies.size(); ++i){
            bool isSelected = (selectedEnemyIndex_ == ( int ) i);

            if (isSelected){
                // 選択が変わったときのみ nameBuf_ を更新
                if (lastSelectedIndex_ != selectedEnemyIndex_){
                    std::string currentName = enemies[i]->GetName();
                    std::snprintf(nameBuf_, sizeof(nameBuf_), "%s", currentName.c_str());
                    lastSelectedIndex_ = selectedEnemyIndex_;
                }

                // 入力フィールドを表示
                if (ImGui::InputText(("##EnemyName" + std::to_string(i)).c_str(),
                    nameBuf_, IM_ARRAYSIZE(nameBuf_))){
                    enemies[i]->Rename(std::string(nameBuf_));
                }
            } else{
                // 表示用ラベル
                std::string label;
                if (enemies[i]->GetName() != "enemy"){
                    label = enemies[i]->GetName();
                } else{
                    label = "Enemy " + std::to_string(i);
                }

                if (ImGui::Selectable(label.c_str(), false)){
                    selectedEnemyIndex_ = i;
                    // 選択が変わったのでフラグをリセット
                    lastSelectedIndex_ = -1;
                }
            }
        }
    }
    ImGui::EndChild();

    //---------------------------------------------------------------------------------
    // 4) 右側の選択中エネミー編集
    //---------------------------------------------------------------------------------
    ImGui::SameLine();
    ImGui::BeginChild("Enemy Editor", ImVec2(250, 300), true);
    {
        auto& enemies = em->GetEnemies();
        if (selectedEnemyIndex_ >= 0 && selectedEnemyIndex_ < ( int ) enemies.size()){
            ImGui::Text("Editing Enemy %d", selectedEnemyIndex_);
            ImGui::Separator();

            auto& enemy = enemies[selectedEnemyIndex_];
            auto pos = enemy->GetWorldTranslate();

            // EnemyごとのカスタムImGui
            enemy->ShowImGui();

            // Position調整
            if (ImGui::DragFloat3("Position", &pos.x, 0.1f)){
                enemy->SetTranslate(pos);
            }

            ImGui::Separator();

            // Removeボタン
            if (ImGui::Button("Remove Selected Enemy")){
                em->DeleteEnemy(selectedEnemyIndex_);
                // インデックスを調整
                if (selectedEnemyIndex_ >= ( int ) em->GetEnemies().size()){
                    selectedEnemyIndex_ = ( int ) em->GetEnemies().size() - 1;
                }
            }
        } else{
            ImGui::Text("No Enemy Selected");
        }
    }
    ImGui::EndChild();

    //---------------------------------------------------------------------------------
    // Editorウィンドウ終了
    //---------------------------------------------------------------------------------
    ImGui::End();


    //---------------------------------------------------------------------------------
    // ルーチンエディタ
    //---------------------------------------------------------------------------------
    ImGui::Begin("Routine Editor");

    ImGui::Separator();
    ImGui::Text("Routine Library Editor:");

    // RoutineManager を取得
    RoutineManager* routineManager = em->GetRoutineManager();
    if (!routineManager){
        ImGui::Text("No Routine Manager available.");
        ImGui::End();
        return;
    }

    // 新規ルーチン作成
    static char newRoutineBuf[64] = "";
    ImGui::InputText("New Routine Name", newRoutineBuf, IM_ARRAYSIZE(newRoutineBuf));
    ImGui::SameLine();
    if (ImGui::Button("Create Routine")){
        std::string newName = newRoutineBuf;
        if (!newName.empty()){
            const auto* existingRoutine = routineManager->GetRoutinePoints(newName);
            if (!existingRoutine){
                routineManager->AddRoutine(newName, {});
                selectedRoutineName = newName;
            }
        }
    }

    uint32_t currentStageNum = StageManager::GetCurrentStageNo();

    // 保存・読み込みボタン
    if (ImGui::Button("Save Routine Library")){
        routineManager->SaveRoutines(currentStageNum);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Routine Library")){
        routineManager->LoadRoutines(currentStageNum);
    }

    // 左側: ルーチン一覧
    ImGui::BeginChild("RoutineList", ImVec2(200, 300), true);
    {
        auto routineNames = routineManager->GetRoutineNames();
        for (const auto& routineName : routineNames){
            bool isSelected = (routineName == selectedRoutineName);
            if (ImGui::Selectable(routineName.c_str(), isSelected)){
                selectedRoutineName = routineName;
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // 右側: 選択中のルーチン編集
    ImGui::BeginChild("RoutineEditor", ImVec2(300, 300), true);
    {
        if (!selectedRoutineName.empty()){
            const auto* points = routineManager->GetRoutinePoints(selectedRoutineName);
            if (points){
                ImGui::Text("Editing Routine: %s", selectedRoutineName.c_str());
                ImGui::Separator();

                auto editablePoints = *points;
                for (size_t i = 0; i < editablePoints.size(); ++i){
                    ImGui::PushID(static_cast< int >(i));
                    if (ImGui::DragFloat3("Point", &editablePoints[i].x, 0.1f)){
                        routineManager->AddRoutine(selectedRoutineName, editablePoints);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Remove")){
                        editablePoints.erase(editablePoints.begin() + i);
                        routineManager->AddRoutine(selectedRoutineName, editablePoints);
                        ImGui::PopID();
                        break;
                    }
                    ImGui::PopID();

                    //routinePointの可視化
                    //routinPointの可視化
                    Sphere s;
                    s.radius = 0.1f;
                    s.center = editablePoints[i];
                    SEED::DrawSphere(s.center, s.radius, 8, {1, 1, 1, 1});
                }

                if (ImGui::Button("Add Point")){
                    editablePoints.push_back({0, 0, 0});
                    routineManager->AddRoutine(selectedRoutineName, editablePoints);
                }

                // マウスでポイント追加
                AddRoutinePointByMouse();

                ImGui::Separator();

                // ルーチン削除
                if (ImGui::Button("Delete Routine")){
                    routineManager->DeleteRoutine(selectedRoutineName);
                    selectedRoutineName.clear();
                }

            } else{
                ImGui::Text("Routine not found!");
            }
        } else{
            ImGui::Text("No routine selected");
        }
    }
    ImGui::EndChild();


    ImGui::End(); // "Routine Editor" ウィンドウ終了

#endif // _DEBUG
}


////////////////////////////////////////////////////////////////////////
// JSON / CSV保存・読み込みなど
////////////////////////////////////////////////////////////////////////
void EnemyEditor::SaveEnemies(){
    Stage* currentStage = StageManager::GetCurrentStage();
    if (!currentStage){ return; }
    EnemyManager* em = currentStage->GetEnemyManager();
    if (!em){ return; }

    em->SaveEnemies();
}

////////////////////////////////////////////////////////////////////////
// マウスによるルーチンポイント追加
////////////////////////////////////////////////////////////////////////
void EnemyEditor::AddRoutinePointByMouse(){
    // 現在のステージとルーチンマネージャを取得
    Stage* currentStage = StageManager::GetCurrentStage();
    if (!currentStage){
        std::cerr << "No current stage available." << std::endl;
        return;
    }

    EnemyManager* em = currentStage->GetEnemyManager();
    if (!em){
        std::cerr << "No EnemyManager available in the current stage." << std::endl;
        return;
    }

    RoutineManager* routineManager = em->GetRoutineManager();
    if (!routineManager){
        std::cerr << "No RoutineManager available." << std::endl;
        return;
    }

    if (selectedRoutineName.empty()){
        std::cerr << "No routine selected for adding points." << std::endl;
        return;
    }

    // ImGuiウィンドウ上をクリックした場合を除外
    Vector3 pointPosition {};

    // 選択されたオブジェクトのGUIDを取得
    std::string selectedGUID = currentStage->GetSelectedObjectGUID();
    if (!selectedGUID.empty()){
        FieldObject* selectedObject = currentStage->GetFieldObjectByGUID(selectedGUID);

        if (selectedObject){
            // オブジェクトが存在する場合のみ座標を取得
            pointPosition = {
                selectedObject->GetWorldTranslate().x,
                selectedObject->GetWorldTranslate().y,
                selectedObject->GetWorldTranslate().z
            };
        } else{
            // 選択GUIDが存在するが、オブジェクトが見つからない場合
            std::cerr << "Selected object with GUID " << selectedGUID << " does not exist." << std::endl;
            currentStage->SetSelectedObjectGUID(""); // 選択をクリア
            return;
        }
    }

    // (0,0,0) は無効なポイントとして扱う
    if (pointPosition == Vector3 {0.0f, 0.0f, 0.0f}){
        std::cerr << "No valid object selected under the mouse position." << std::endl;
        return;
    }

    if (Input::IsTriggerKey(DIK_SPACE)){
        // ルーチンポイントとして追加
        auto* points = routineManager->GetRoutinePoints(selectedRoutineName);
        if (points){
            std::vector<Vector3> editablePoints = *points; // 現在のポイントを取得
            editablePoints.push_back(pointPosition);       // 新しいポイントを追加
            routineManager->AddRoutine(selectedRoutineName, editablePoints); // 更新
            std::cout << "Point added to routine: " << selectedRoutineName << " at "
                << pointPosition.x << ", " << pointPosition.y << ", " << pointPosition.z << std::endl;
        } else{
            std::cerr << "Failed to retrieve routine points for: " << selectedRoutineName << std::endl;
        }
    }
}

