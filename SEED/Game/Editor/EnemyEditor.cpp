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
                    enemies[i]->Rename(std::string(nameBuf_));
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

            enemy->ShowImGui();

            // Position
            if (ImGui::DragFloat3("Position", &pos.x, 0.1f)){
                enemy->SetPosition(pos);
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

    ImGui::End();

    //////////////////////////////////////////////////////////////////////////////////////////
    // ルーチンエディタの表示
    //////////////////////////////////////////////////////////////////////////////////////////

    ImGui::Begin("Routine Editor");

    // ルーチン編集 UI の開始
    ImGui::Separator();
    ImGui::Text("Routine Library Editor:");


    // 選択中のルーチン名を保持するための静的変数
    static std::string selectedRoutineName;

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

   

    ImGui::End();

#endif // _DEBUG
}


////////////////////////////////////////////////////////////////////////
//          json関連
////////////////////////////////////////////////////////////////////////
void EnemyEditor::SaveEnemies(){
    //-----------------------------------
    // 1) 敵一覧を取得
    //-----------------------------------
    auto& enemies = pEnemyManager_->GetEnemies();

    //-----------------------------------
    // 2) CSV (座標) の作成と保存
    //-----------------------------------
    std::vector<std::vector<std::string>> csvData;
    csvData.push_back({"Index", "Name", "PosX", "PosY", "PosZ"}); // ヘッダ行

    for (int i = 0; i < ( int ) enemies.size(); ++i){
        auto& e = enemies[i];
        auto pos = e->GetWorldTranslate();

        // CSV 行を作成してpush_back
        csvData.push_back({
            std::to_string(i),
            e->GetName(),
            std::to_string(pos.x),
            std::to_string(pos.y),
            std::to_string(pos.z)
                          });
    }
    // CSV保存
    CsvAdapter::GetInstance()->SaveCsv("enemies_position", csvData);

    //-----------------------------------
    // 3) JSON の作成
    //-----------------------------------
    nlohmann::json rootJson;
    // 敵数を記録
    rootJson["Count"] = static_cast< int >(enemies.size());

    // 敵情報の配列
    nlohmann::json enemyArray = nlohmann::json::array();

    for (int i = 0; i < ( int ) enemies.size(); ++i){
        auto& e = enemies[i];
        if (!e) continue;

        // 1体分の Enemy 情報を json オブジェクトに詰める
        nlohmann::json enemyObj;
        enemyObj["Index"] = i;
        enemyObj["Name"] = e->GetName();
        enemyObj["HP"] = e->GetHP();
        enemyObj["CanEat"] = e->GetCanEat();
        enemyObj["ChasePlayer"] = e->GetChasePlayer();
        enemyObj["RoutineName"] = e->GetRoutineName();

        // 必要に応じて、座標情報も JSON に入れるなら
        // enemyObj["PosX"] = pos.x; など書いても構いません。
        // (今は CSV に保存しているので省略)

        enemyArray.push_back(enemyObj);
    }

    // ルートに配列をセット
    rootJson["Enemies"] = enemyArray;

    //-----------------------------------
    // 4) JSON ファイルとして出力
    //-----------------------------------
    // Enemies/Enemies.json のようなパスに保存するなら適宜調整
    std::string filePath = "resources/jsons/enemies/enemies.json";
    try{
        std::ofstream ofs(filePath);
        if (!ofs){
            // ファイルが開けなかった場合のエラー処理
            std::cerr << "Failed to open file for writing: " << filePath << std::endl;
            return;
        }
        // インデント4で整形
        ofs << rootJson.dump(4) << std::endl;
        ofs.close();
    } catch (const std::exception& e){
        std::cerr << "Exception while saving JSON: " << e.what() << std::endl;
    }

    // （オマケ）編集用のメンバ変数にも敵数を保存
    enemyCount_ = static_cast< int >(enemies.size());
}


void EnemyEditor::LoadEnemies(){
    // ※ CSV 側の読み込みは従来どおり（位置情報用）
    auto csvData = CsvAdapter::GetInstance()->LoadCsv("enemies_position");
    if (csvData.size() <= 1){
        // 空なら何もしない
        return;
    }

    // JSON ファイルを読み込む (Enemies/Enemies.json 等)
    nlohmann::json rootJson;
    {
        std::string filePath = "resources/jsons/enemies/enemies.json";
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

    // まず古い敵を全削除
    pEnemyManager_->ClearAllEnemies();

    // JSON から "Count" を取得 (無ければ終了)
    if (!rootJson.contains("Count")){
        std::cerr << "JSON has no 'Count' field." << std::endl;
        return;
    }
    int count = rootJson["Count"].get<int>();

    // "Enemies" 配列もチェック
    if (!rootJson.contains("Enemies") || !rootJson["Enemies"].is_array()){
        std::cerr << "JSON has no 'Enemies' array." << std::endl;
        return;
    }
    auto enemyArray = rootJson["Enemies"];

    // CSV は行1以降にデータがあるので i=1 からループ
    for (size_t i = 1; i < csvData.size(); ++i){
        auto& row = csvData[i];
        if (row.size() < 5){
            continue; // 不正行はスキップ
        }
        // CSV カラムをパース
        int index = std::stoi(row[0]);
        std::string eName = row[1];
        float px = std::stof(row[2]);
        float py = std::stof(row[3]);
        float pz = std::stof(row[4]);

        // 新しい Enemy を生成
        auto newEnemy = std::make_unique<Enemy>(
            pEnemyManager_,
            pEnemyManager_->GetPlayer(),
            eName
        );
        // 位置だけは CSV で管理しているのでセット
        newEnemy->SetPosition({px, py, pz});

        // JSON から他パラメータ(HP, canEat 等)を取得
        // index < count && index < enemyArray.size() なら
        if (index < count && index < ( int ) enemyArray.size()){
            auto& eJson = enemyArray[index];
            // is_object() かどうか等のチェックをしておくと安全
            if (eJson.is_object()){
                // HP (無ければデフォルト100)
                int hp = eJson.value("HP", 100);
                newEnemy->SetHP(hp);

                // canEat
                bool canEat = eJson.value("CanEat", false);
                newEnemy->SetCanEat(canEat);

                // chasePlayer
                bool chasePlayer = eJson.value("ChasePlayer", false);
                newEnemy->SetChasePlayer(chasePlayer);

                // routineName
                std::string routineName = eJson.value("RoutineName", "NULL");
                newEnemy->SetRoutineName(routineName);
            }
        }

        // Manager に追加
        pEnemyManager_->GetEnemies().push_back(std::move(newEnemy));
    }

    // 敵が一体以上いるなら最初の敵を選択
    selectedEnemyIndex_ = (pEnemyManager_->GetEnemies().empty()) ? -1 : 0;
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