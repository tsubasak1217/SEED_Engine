#include "EnemyEditor.h"

// local
#include "../Manager/EnemyManager.h"

// lib
#include "JsonManager/JsonCoordinator.h"
#include "imgui.h"

EnemyEditor::EnemyEditor(EnemyManager* manager)
:pEnemyManager_ (manager){
    // グループ名を定義
    const std::string group = "Enemies";

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
    ImGui::BeginChild("Enemy List", ImVec2(200, 300), true); // 幅200,高さ300のスクロール可能領域
    {
        ImGui::Text("Enemies:");

        auto& enemies = pEnemyManager_->GetEnemies();
        for (uint32_t i = 0; i < static_cast< int >(enemies.size()); ++i){
            // ラベル例: "Enemy 0", "Enemy 1", ...
            std::string label = "Enemy " + std::to_string(i);

            // 選択中かどうか
            bool isSelected = (selectedEnemyIndex_ == i);

            // 選択中の敵は色を変える・ハイライトするなど
            if (isSelected){
                // 例えばテキストカラーを変更
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.f));
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.4f, 0.f, 1.f));
                ImGui::Selectable(label.c_str(), true);
                ImGui::PopStyleColor(2);
            } else{
                if (ImGui::Selectable(label.c_str(), false)){
                    selectedEnemyIndex_ = i; // クリックで選択
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

    // ウィンドウ終了
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
        auto newEnemy = std::make_unique<Enemy>(pEnemyManager_->GetPlayer());
        newEnemy->SetPosition(pos);
        newEnemy->SetHP(hp);

        // EnemyManagerに追加
        pEnemyManager_->GetEnemies().push_back(std::move(newEnemy));
    }

    // 読み込み終了後、選択インデックスをリセット
    selectedEnemyIndex_ = 0;
}