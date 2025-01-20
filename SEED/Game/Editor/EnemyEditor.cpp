#include "EnemyEditor.h"

// local
#include "../Manager/EnemyManager.h"

// lib
#include "../adapter/json/JsonCoordinator.h"
#include "../adapter/csv/CsvAdapter.h"
#include "imgui.h"

EnemyEditor::EnemyEditor(EnemyManager* manager)
:pEnemyManager_ (manager){
    // グループ名を定義
    const std::string group = "Enemies";

    LoadRoutineLibrary();
    LoadEnemies();
}

void EnemyEditor::ShowImGui(){
#ifdef _DEBUG
    // ウィンドウ開始
    ImGui::Begin("Enemy Editor");

    // --------------------------------------------------------
    // 2) 上部に「Add Enemy」「Save」ボタンなどを配置
    // --------------------------------------------------------
    if (ImGui::Button("Add Enemy")){
        // 例: EnemyManager 経由で敵を追加
        pEnemyManager_->AddEnemy();
        // リストの最後を選択状態にする
        selectedEnemyIndex_ = static_cast< int >(pEnemyManager_->GetEnemies().size()) - 1;
    }
    ImGui::SameLine();

    if (ImGui::Button("Save Enemies")){
        // セーブ処理 (JSONなど) 
        SaveEnemies();  
    }

    ImGui::Separator();

    // --------------------------------------------------------
    // 3) 左側: 敵リスト表示用Child
    // --------------------------------------------------------
    ImGui::BeginChild("Enemy List", ImVec2(200, 300), true);
    {
        ImGui::Text("Enemies:");
        auto& enemies = pEnemyManager_->GetEnemies();
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
                if (ImGui::InputText(("##EnemyName" + std::to_string(i)).c_str(), nameBuf_, IM_ARRAYSIZE(nameBuf_))){
                    enemies[i]->SetName(std::string(nameBuf_));
                }
            } else{
                std::string label;
                if (enemies[i]->GetName() != "enemy"){
                    label = enemies[i]->GetName();
                } else{
                    label = "Enemy " + std::to_string(i);
                }

                if (ImGui::Selectable(label.c_str(), false)){
                    selectedEnemyIndex_ = i;
                    // 選択が変わったので更新フラグをセット
                    lastSelectedIndex_ = -1;
                }
            }
        }
    }
    ImGui::EndChild();


    // --------------------------------------------------------
    // 4) 右側: 選択中の敵を編集するUI
    // --------------------------------------------------------
    ImGui::SameLine(); // 同じ行に配置して左右に並べる

    ImGui::BeginChild("Enemy Editor", ImVec2(250, 300), true); // 幅250,高さ300
    {
        auto& enemies = pEnemyManager_->GetEnemies();
        if (selectedEnemyIndex_ >= 0 && selectedEnemyIndex_ < static_cast< int >(enemies.size())){
            // 選択中の Enemy があればパラメータ編集UIを表示
            ImGui::Text("Editing Enemy %d", selectedEnemyIndex_);
            ImGui::Separator();

            // 例: Position, HP を編集
            auto& enemy = enemies[selectedEnemyIndex_];
            auto pos = enemy->GetWorldTranslate();
            int32_t hp = enemy->GetHP();

            // Position
            if (ImGui::DragFloat3("Position", &pos.x, 0.1f)){
                enemy->SetPosition(pos);
            }
            // HP
            if (ImGui::DragInt("HP", &hp, 1, 0, 100)){
                enemy->SetHP(hp);
            }

            ImGui::Separator();
            // 削除ボタン
            if (ImGui::Button("Remove Selected Enemy")){
                pEnemyManager_->DeleteEnemy(selectedEnemyIndex_);
                // リストが縮んだので選択インデックスを調整
                if (selectedEnemyIndex_ >= static_cast< int >(pEnemyManager_->GetEnemies().size())){
                    selectedEnemyIndex_ = static_cast< int >(pEnemyManager_->GetEnemies().size()) - 1;
                }
            }
        } else{
            // 敵が選ばれていない or リストが空
            ImGui::Text("No Enemy Selected");
        }
    }
    ImGui::EndChild();

    // ルーチン編集 UI の開始
    ImGui::Separator();
    ImGui::Text("Routine Library Editor:");

    // 選択中のルーチン名を保持するための静的変数
    static std::string selectedRoutineName;

    // 左側: ルーチン一覧表示
    ImGui::BeginChild("RoutineList", ImVec2(200, 300), true);
    {
        auto& library = pEnemyManager_->GetRoutineLibrary();
        for (auto& kv : library){
            const std::string& routineName = kv.first;
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
        auto& library = pEnemyManager_->GetRoutineLibrary();
        if (!selectedRoutineName.empty()){
            auto it = library.find(selectedRoutineName);
            if (it != library.end()){
                auto& points = it->second;
                ImGui::Text("Editing Routine: %s", selectedRoutineName.c_str());
                ImGui::Separator();

                // 各制御点を編集
                for (size_t i = 0; i < points.size(); ++i){
                    ImGui::PushID(( int ) i);
                    if (ImGui::DragFloat3("Point", &points[i].x, 0.1f)){
                        // 座標が変更されたときの処理（必要なら）
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Remove")){
                        points.erase(points.begin() + i);
                        ImGui::PopID();
                        break; // 削除後にループを抜けて再描画
                    }
                    ImGui::PopID();
                }

                if (ImGui::Button("Add Point")){
                    points.push_back({0,0,0});
                }
                ImGui::Separator();

                // ルーチンの削除
                if (ImGui::Button("Delete Routine")){
                    library.erase(it);
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

    // 新規ルーチン作成の UI
    static char newRoutineBuf[64] = "";
    ImGui::InputText("New Routine Name", newRoutineBuf, IM_ARRAYSIZE(newRoutineBuf));
    ImGui::SameLine();
    if (ImGui::Button("Create Routine")){
        std::string newName = newRoutineBuf;
        if (!newName.empty()){
            auto& library = pEnemyManager_->GetRoutineLibrary();
            if (library.find(newName) == library.end()){
                library[newName] = {}; // 空の制御点リストを作成
                selectedRoutineName = newName;
            }
        }
    }

    // 保存・読み込みボタン
    if (ImGui::Button("Save Routine Library")){
        SaveRoutineLibrary();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Routine Library")){
        LoadRoutineLibrary();
    }

    ImGui::End();

#endif // _DEBUG
}


////////////////////////////////////////////////////////////////////////
//          json関連
////////////////////////////////////////////////////////////////////////
void EnemyEditor::SaveEnemies(){
    // グループ名を定義
    const std::string group = "Enemies";

    // 1) まず現在のグループ情報をメモリに展開(ファイルが無ければ新規作成)
    JsonCoordinator::LoadGroup(group);

    // 2) 敵の一覧を取得
    
    auto& enemies = pEnemyManager_->GetEnemies();
   
    // 敵の情報を1体ずつ書き込む
    for (int i = 0; i < ( int ) enemies.size(); i++){
        // キー文字列
        std::string posKey = "Enemy_" + std::to_string(i) + "_Position";
        std::string hpKey = "Enemy_" + std::to_string(i) + "_HP";

        // 値を取得
        auto& e = enemies[i];
        auto pos = e->GetWorldTranslate();
        int32_t hp = e->GetHP();

        // SetValue
        JsonCoordinator::RegisterItem(group, posKey, pos);
        JsonCoordinator::RegisterItem(group, hpKey, hp);
    }
    enemyCount_ = ( int ) pEnemyManager_->GetEnemies().size();
    // 敵数をセット
    const std::string countKey = "Count";
    JsonCoordinator::RegisterItem(group, countKey, enemyCount_);

    // 3) SaveGroup でファイルに保存
    JsonCoordinator::SaveGroup(group);
}

//-----------------------------------------------------
// (重要) エディタ内でロード機能を実装
//-----------------------------------------------------
void EnemyEditor::LoadEnemies(){
    const std::string group = "Enemies";

    // 1) グループを読み込む (ファイルが無い場合は false)
    if (!JsonCoordinator::LoadGroup(group)){
        // ファイルがない(初回など)は何もしないか、新規生成する
        return;
    }

    // 2) "Count" (敵の数)を取得
    auto countOpt = JsonCoordinator::GetValue(group, "Count");
    if (!countOpt){
        // 無ければ何もしない
        return;
    }
    int count = std::get<int>(*countOpt);

    // 3) いったん既存の敵を全部消すかどうかは好み
    pEnemyManager_->ClearAllEnemies();

    // 4) 敵を数だけ復元
    for (int i = 0; i < count; i++){
        std::string posKey = "Enemy_" + std::to_string(i) + "_Position";
        std::string hpKey = "Enemy_" + std::to_string(i) + "_HP";

        // キーから値を取得
        auto posOpt = JsonCoordinator::GetValue(group, posKey);
        auto hpOpt = JsonCoordinator::GetValue(group, hpKey);

        // どちらかが存在しない場合はスキップ
        if (!posOpt || !hpOpt){
            continue;
        }

        // 値を取り出す
        Vector3 pos = std::get<Vector3>(*posOpt);
        int32_t   hp = std::get<int32_t>(*hpOpt);

        // 新しいEnemyを生成
        //! TODO : Enemy Name を Editor で 編集,初期化時に Name を渡す
        auto newEnemy = std::make_unique<Enemy>(pEnemyManager_,pEnemyManager_->GetPlayer(),"Default");
        newEnemy->SetPosition(pos);
        newEnemy->SetHP(hp);

        // EnemyManagerに追加
        pEnemyManager_->GetEnemies().push_back(std::move(newEnemy));
    }

    // 読み込み終了後、選択インデックスをリセット
    selectedEnemyIndex_ = 0;
}

////////////////////////////////////////////////////////////////////////
//          ルーチンライブラリのセーブ
////////////////////////////////////////////////////////////////////////
void EnemyEditor::SaveRoutineLibrary(){
    // CSV に書き出すためのデータ構造を準備
    std::vector<std::vector<std::string>> csvData;

    // ヘッダ行（任意）
    csvData.push_back({"RoutineName", "PointCount", "Points..."});

    auto& library = pEnemyManager_->GetRoutineLibrary();
    for (auto& kv : library){
        const std::string& routineName = kv.first;
        const auto& points = kv.second;

        std::vector<std::string> row;
        row.push_back(routineName);
        row.push_back(std::to_string(points.size()));

        for (auto& p : points){
            row.push_back(std::to_string(p.x));
            row.push_back(std::to_string(p.y));
            row.push_back(std::to_string(p.z));
        }
        csvData.push_back(row);
    }

    // CsvAdapter のシングルトンインスタンスを取得して保存
    CsvAdapter::GetInstance()->SaveCsv("routine_library", csvData);
}

////////////////////////////////////////////////////////////////////////
//          ルーチンライブラリのロード
////////////////////////////////////////////////////////////////////////
void EnemyEditor::LoadRoutineLibrary(){
    // CsvAdapter を使って CSV ファイルを読み込む
    auto csvData = CsvAdapter::GetInstance()->LoadCsv("routine_library");
    if (csvData.size() <= 1){
        // データがない、またはヘッダのみの場合は何もしない
        return;
    }

    auto& library = pEnemyManager_->GetRoutineLibrary();
    library.clear();

    // 先頭行はヘッダと仮定してスキップ
    for (size_t i = 1; i < csvData.size(); ++i){
        auto& row = csvData[i];
        if (row.size() < 2) continue; // 必要なデータがない行はスキップ

        std::string routineName = row[0];
        int pointCount = std::stoi(row[1]);
        std::vector<Vector3> points;
        points.reserve(pointCount);

        size_t expectedSize = 2 + 3 * pointCount;
        if (row.size() < expectedSize) continue; // データ不足の場合はスキップ

        size_t idx = 2;
        for (int j = 0; j < pointCount; ++j){
            Vector3 p;
            p.x = std::stof(row[idx++]);
            p.y = std::stof(row[idx++]);
            p.z = std::stof(row[idx++]);
            points.push_back(p);
        }

        library[routineName] = points;
    }
}