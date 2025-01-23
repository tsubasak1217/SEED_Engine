#include "EnemyManager.h"

#include "../adapter/csv/CsvAdapter.h"

EnemyManager::EnemyManager(){
    enemies_.clear();
}

EnemyManager::EnemyManager(Player* player, uint32_t stageNo)
:pPlayer_(player),stageNo_(stageNo){
    routineManager_.LoadRoutines(stageNo);
}



void EnemyManager::Initialize(){}

void EnemyManager::Update(){
    for(auto& enemy : enemies_){
        enemy->Update();
    }

    //死亡した敵を削除
    std::erase_if(enemies_,[](const std::unique_ptr<Enemy>& enemy){
        return !enemy->GetIsAlive();
                  });
}

void EnemyManager::Draw(){
    for(auto& enemy : enemies_){
        enemy->Draw();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//      enemyの追加
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::AddEnemy(){
    if(!pPlayer_){ assert(false); }
    //enemyの生成
    const std::string enemyName = "Enemy" + std::to_string((int)enemies_.size());
    auto newEnemy = std::make_unique<Enemy>(this,pPlayer_,enemyName);
    enemies_.emplace_back(std::move(newEnemy));
}

void EnemyManager::AddEnemy(std::unique_ptr<Enemy>enemy){
    if(!pPlayer_){ assert(false); }
    enemies_.emplace_back(std::move(enemy));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//      enemyの削除
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::DeleteEnemy(uint32_t index){
    if(index < enemies_.size()){
        enemies_.erase(enemies_.begin() + index);
    }
}

void EnemyManager::HandOverColliders(){
    for(auto& enemy : enemies_){
        enemy->HandOverColliders();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//      enemyの保存
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::SaveEnemies(){
    // ===== CSVで座標を保存 (従来通り) =====
    std::vector<std::vector<std::string>> csvData;
    csvData.emplace_back(std::vector<std::string>{"Index", "Name", "PosX", "PosY", "PosZ"}); // ヘッダ行

    for (size_t i = 0; i < enemies_.size(); ++i){
        auto& e = enemies_[i];
        if (!e) continue;
        auto pos = e->GetWorldTranslate();
        csvData.emplace_back(std::vector<std::string>{
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
    rootJson["Count"] = static_cast< int >(enemies_.size());

    nlohmann::json enemyArray = nlohmann::json::array();
    for (size_t i = 0; i < enemies_.size(); ++i){
        auto& e = enemies_[i];
        if (!e) continue;
        nlohmann::json enemyObj;
        enemyObj["Index"] = static_cast< int >(i);
        enemyObj["Name"] = e->GetName();
        enemyObj["HP"] = e->GetHP();
        enemyObj["CanEat"] = e->GetCanEat();
        enemyObj["ChasePlayer"] = e->GetChasePlayer();
        enemyObj["RoutineName"] = e->GetRoutineName();
        enemyObj["moveSpeed"] = e->GetMoveSpeed();
        // 座標をJSONにも入れるなら
        // auto pos = e->GetWorldTranslate();
        // enemyObj["PosX"] = pos.x; ...
        enemyArray.push_back(enemyObj);
    }
    rootJson["Enemies"] = enemyArray;

    // ===== JSONファイル出力 =====
    std::string filePath = "resources/jsons/enemies/stage_"
        + std::to_string(stageNo_)
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

    // 敵数をメンバ変数に記録
    enemyCount_ = static_cast< int >(enemies_.size());
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//      enemyの読み込み
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void EnemyManager::LoadEnemies(){
    // ===== CSV読み込み =====
    auto csvData = CsvAdapter::GetInstance()->LoadCsv("enemies_position");
    if (csvData.size() <= 1){
        // 空なら何もしない
        return;
    }

    // ===== JSON読み込み =====
    nlohmann::json rootJson;
    {
        std::string filePath = "resources/jsons/enemies/stage_"
            + std::to_string(stageNo_)
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
    ClearAllEnemies();

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
            this,        // EnemyManager*
            pPlayer_,    // Player*
            eName
        );
        // CSVの位置をセット
        newEnemy->SetPosition({px, py, pz});

        // JSONから他のパラメータをセット
        if (index < count && index < static_cast< int >(enemyArray.size())){
            auto& eJson = enemyArray[index];
            if (eJson.is_object()){
                int hp = eJson.value("HP", 100);
                newEnemy->SetHP(hp);

                bool canEat = eJson.value("CanEat", false);
                newEnemy->SetCanEat(canEat);

                bool chasePlayer = eJson.value("ChasePlayer", false);
                newEnemy->SetChasePlayer(chasePlayer);

                float moveSpeed = eJson.value("moveSpeed", 1.0f);
                newEnemy->SetMoveSpeed(moveSpeed);

                std::string routineName = eJson.value("RoutineName", "NULL");
                newEnemy->SetRoutineName(routineName);
            }
        }
        newEnemy->InitializeRoutine();
        newEnemy->UpdateMatrix();
        enemies_.emplace_back(std::move(newEnemy));
    }


    enemyCount_ = static_cast< int >(enemies_.size());
}