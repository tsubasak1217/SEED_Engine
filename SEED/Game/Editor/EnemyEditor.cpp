#include "EnemyEditor.h"

// local
#include "../Manager/EnemyManager.h"
#include "../Manager/StageManager.h"

// lib
#include "../adapter/json/JsonCoordinator.h"
#include "../adapter/csv/CsvAdapter.h"
#include "imgui.h"

EnemyEditor::EnemyEditor(){
    // Editor起動時にロードするなら (任意)
    LoadEnemies();
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
    if (ImGui::Button("Add Enemy")){
        em->AddEnemy();
        selectedEnemyIndex_ = static_cast< int >(em->GetEnemies().size()) - 1;
    }
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
                enemy->SetPosition(pos);
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

    // EnemyRoutineManager を取得
    EnemyRoutineManager* routineManager = em->GetRoutineManager();
    if (!routineManager){
        ImGui::Text("No Routine Manager available.");
        ImGui::End();
        return;
    }

    // 選択中のルーチン名を静的に保持
    static std::string selectedRoutineName;

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
                        break; // 再描画のためループを抜ける
                    }
                    ImGui::PopID();
                }

                if (ImGui::Button("Add Point")){
                    editablePoints.push_back({0, 0, 0});
                    routineManager->AddRoutine(selectedRoutineName, editablePoints);
                }
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
    // 現在のステージや EnemyManager を取得
    Stage* currentStage = StageManager::GetCurrentStage();
    if (!currentStage){ return; }
    EnemyManager* em = currentStage->GetEnemyManager();
    if (!em){ return; }

    auto& enemies = em->GetEnemies();

    // ステージ番号を取得
    int stageNo = StageManager::GetCurrentStageNo();

    // ===== CSVで座標を保存 (従来通り) =====
    std::vector<std::vector<std::string>> csvData;
    csvData.push_back({"Index", "Name", "PosX", "PosY", "PosZ"}); // ヘッダ行

    for (int i = 0; i < ( int ) enemies.size(); ++i){
        auto& e = enemies[i];
        auto pos = e->GetWorldTranslate();
        csvData.push_back({
            std::to_string(i),
            e->GetName(),
            std::to_string(pos.x),
            std::to_string(pos.y),
            std::to_string(pos.z)
                          });
    }
    CsvAdapter::GetInstance()->SaveCsv("enemies_position", csvData);

    // ===== JSONの作成 =====
    nlohmann::json rootJson;
    rootJson["Count"] = ( int ) enemies.size();

    nlohmann::json enemyArray = nlohmann::json::array();
    for (int i = 0; i < ( int ) enemies.size(); ++i){
        auto& e = enemies[i];
        if (!e) continue;
        nlohmann::json enemyObj;
        enemyObj["Index"] = i;
        enemyObj["Name"] = e->GetName();
        enemyObj["HP"] = e->GetHP();
        enemyObj["CanEat"] = e->GetCanEat();
        enemyObj["ChasePlayer"] = e->GetChasePlayer();
        enemyObj["RoutineName"] = e->GetRoutineName();
        // 座標をJSONにも入れるなら
        // auto pos = e->GetWorldTranslate();
        // enemyObj["PosX"] = pos.x; ...

        enemyArray.push_back(enemyObj);
    }
    rootJson["Enemies"] = enemyArray;

    // ===== JSONファイル出力 =====
    std::string filePath = "resources/jsons/enemies/stage_"
        + std::to_string(stageNo)
        + "_enemies.json";
    try{
        std::ofstream ofs(filePath);
        if (!ofs){
            std::cerr << "Failed to open file for writing: " << filePath << std::endl;
            return;
        }
        ofs << rootJson.dump(4) << std::endl;
        ofs.close();
    } catch (const std::exception& e){
        std::cerr << "Exception while saving JSON: " << e.what() << std::endl;
    }

    // 敵数をメンバ変数に記録(オマケ)
    enemyCount_ = ( int ) enemies.size();
}

void EnemyEditor::LoadEnemies(){
    // 現在ステージや EnemyManager を取得
    Stage* currentStage = StageManager::GetCurrentStage();
    if (!currentStage){ return; }
    EnemyManager* em = currentStage->GetEnemyManager();
    if (!em){ return; }

    // ===== CSV読み込み =====
    auto csvData = CsvAdapter::GetInstance()->LoadCsv("enemies_position");
    if (csvData.size() <= 1){
        // 空なら何もしない
        return;
    }

    // ===== JSON読み込み =====
    nlohmann::json rootJson;
    {
        int stageNo = StageManager::GetCurrentStageNo();
        std::string filePath = "resources/jsons/enemies/stage_"
            + std::to_string(stageNo)
            + "_enemies.json";
        std::ifstream ifs(filePath);
        if (!ifs.is_open()){
            std::cerr << "Failed to open JSON file: " << filePath << std::endl;
            return;
        }
        try{
            ifs >> rootJson; // パース
        } catch (const std::exception& e){
            std::cerr << "Exception while reading JSON: " << e.what() << std::endl;
            return;
        }
        ifs.close();
    }

    // まず全削除
    em->ClearAllEnemies();

    // "Count"
    if (!rootJson.contains("Count")){
        std::cerr << "JSON has no 'Count' field." << std::endl;
        return;
    }
    int count = rootJson["Count"].get<int>();

    if (!rootJson.contains("Enemies") || !rootJson["Enemies"].is_array()){
        std::cerr << "JSON has no 'Enemies' array." << std::endl;
        return;
    }
    auto enemyArray = rootJson["Enemies"];

    // ===== CSVの行1以降を走査 =====
    for (size_t i = 1; i < csvData.size(); ++i){
        auto& row = csvData[i];
        if (row.size() < 5){
            continue;
        }
        int index = std::stoi(row[0]);
        std::string eName = row[1];
        float px = std::stof(row[2]);
        float py = std::stof(row[3]);
        float pz = std::stof(row[4]);

        // 新しい Enemy
        auto newEnemy = std::make_unique<Enemy>(
            em,        // EnemyManager*
            em->GetPlayer(), // 必要なら
            eName
        );
        // CSVの位置をセット
        newEnemy->SetPosition({px, py, pz});

        // JSONから他のパラメータをセット
        if (index < count && index < ( int ) enemyArray.size()){
            auto& eJson = enemyArray[index];
            if (eJson.is_object()){
                int hp = eJson.value("HP", 100);
                newEnemy->SetHP(hp);

                bool canEat = eJson.value("CanEat", false);
                newEnemy->SetCanEat(canEat);

                bool chasePlayer = eJson.value("ChasePlayer", false);
                newEnemy->SetChasePlayer(chasePlayer);

                std::string routineName = eJson.value("RoutineName", "NULL");
                newEnemy->SetRoutineName(routineName);
            }
        }

        em->GetEnemies().push_back(std::move(newEnemy));
    }

    selectedEnemyIndex_ = (em->GetEnemies().empty()) ? -1 : 0;
}

