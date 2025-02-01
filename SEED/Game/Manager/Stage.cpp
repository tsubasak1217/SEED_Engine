#include "Stage.h"
#include "CollisionManaer/CollisionManager.h"
#include "EventState/EventFunctionTable.h"
#include "Player/Player.h"

#include "StageManager.h"

// fieldObject
#include "FieldObject/MoveFloor/FieldObject_MoveFloor.h"

//lib
#include <nlohmann/json.hpp>
#include <fstream>

Stage::Stage(ISubject& subject,uint32_t stageNo)
    :subject_(subject){
    // ステージの初期化
    ClearAllFieldObjects();

    stageNo_ = stageNo;

    routineManager_.LoadRoutines(stageNo);

    enemyManager_ = std::make_unique<EnemyManager>(pPlayer_,stageNo_,routineManager_);

    playerCorpseManager_ = std::make_unique<PlayerCorpseManager>();
    playerCorpseManager_->Initialize();
}

void Stage::InitializeStatus(){
    std::string filePath = "resources/jsons/Stages/stage_" + std::to_string(stageNo_ + 1) + ".json";
    InitializeStatus(filePath);
}

void Stage::InitializeStatus(const std::string& _jsonFilePath){
    //======================== オブジェクトの初期化 ========================//
    // 特殊オブジェクトを nullptr に初期化
    startObject_ = nullptr;
    goalObject_ = nullptr;
    starObjects_.clear();

    // Jsonから読み込み
    LoadFromJson(_jsonFilePath);

    // 敵の初期化
    enemyManager_->LoadEnemies();

    // player
    { //卵の所持状況を初期化
        pPlayer_->GetEggManager()->InitializeEggCount();
    }

    //======================== その他 ========================//
    currentStarCount_ = 0;
}

////////////////////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////////////////////
void Stage::Update(){
    for(auto& fieldObject : fieldObjects_){
        // 通常のオブジェクト
        fieldObject->Update();
    }

    // 敵の更新
    enemyManager_->Update();

    // プレイヤーの死体の更新
    playerCorpseManager_->Update();
}

////////////////////////////////////////////////////////////////////////
// 描画関数
////////////////////////////////////////////////////////////////////////
void Stage::Draw(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->Draw();
    }

    // 敵の描画
    enemyManager_->Draw();

    // プレイヤーの死体の描画
    playerCorpseManager_->Draw();
}

////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
////////////////////////////////////////////////////////////////////////
void Stage::BeginFrame(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->BeginFrame();
    }

    playerCorpseManager_->BeginFrame();
}

////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
////////////////////////////////////////////////////////////////////////
void Stage::EndFrame(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->EndFrame();
    }

    // 削除依頼のあるオブジェクトを削除
    fieldObjects_.erase(
        std::remove_if(
        fieldObjects_.begin(),
        fieldObjects_.end(),
        [](const std::unique_ptr<FieldObject>& fieldObject){
            return fieldObject->GetRemoveFlag();
        }
    ),
        fieldObjects_.end()
    );

    // 敵のフレーム終了処理
    enemyManager_->EndFrame();

    // プレイヤーの死体のフレーム終了処理
    playerCorpseManager_->EndFrame();
}

////////////////////////////////////////////////////////////////////////
// すべてのオブジェクトをクリア
////////////////////////////////////////////////////////////////////////
void Stage::ClearAllFieldObjects(){
    // オブジェクトをクリアする前に、オブザーバー登録を解除
    for(auto& obj : fieldObjects_){
        IObserver* observer = dynamic_cast<IObserver*>(obj.get());
        if(observer){
            subject_.UnregisterObserver(observer);
        }
    }
    fieldObjects_.clear();
}

////////////////////////////////////////////////////////////////////////
// 新しいオブジェクトを追加
////////////////////////////////////////////////////////////////////////
void Stage::AddFieldObject(std::unique_ptr<FieldObject> obj){
    // オブジェクトが IObserver を実装している場合、EventManager に登録
    IObserver* observer = dynamic_cast<IObserver*>(obj.get());
    if(observer){
        subject_.RegisterObserver(observer);
    }

    fieldObjects_.push_back(std::move(obj));
}

void Stage::RemoveFieldObject(FieldObject* objToRemove){
    auto it = std::remove_if(fieldObjects_.begin(),fieldObjects_.end(),
                             [&](const std::unique_ptr<FieldObject>& objPtr){
                                 return objPtr.get() == objToRemove;
                             });
    if(it != fieldObjects_.end()){
        // 選択オブジェクトが削除対象の場合、選択をクリア
        if(selectedObjectGUID_ == objToRemove->GetGUID()){
            selectedObjectGUID_.clear();
        }
        fieldObjects_.erase(it,fieldObjects_.end());
    }
}

////////////////////////////////////////////////////////////////////////
// CollisionManagerにコライダーを渡す
////////////////////////////////////////////////////////////////////////
void Stage::HandOverColliders(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->HandOverColliders();
    }

    enemyManager_->HandOverColliders();
}


Vector3 Stage::GetStartPosition() const{
    if(startObject_){
        return startObject_->GetWorldTranslate() + Vector3{0.0f,1.0f,0.0f};
    }
    // スタートオブジェクトが見つからなかった場合のデフォルト値を返す
    return Vector3{0.0f,0.0f,0.0f};
}

// ゴールオブジェクトを取得
FieldObject_Goal* Stage::GetGoalObject() const{
    for(const auto& obj : fieldObjects_){
        // FieldObject_Goal 型へのキャストを試みる
        if(auto* goal = dynamic_cast<FieldObject_Goal*>(obj.get())){
            // ゴールオブジェクトが見つかったら返す
            return goal;
        }
    }
    return nullptr;
}

FieldObject_ViewPoint* Stage::GetViewPoint() const{
    for(const auto& obj : fieldObjects_){
        // FieldObject_Start 型へのキャストを試みる
        if(auto* viewPoint = dynamic_cast<FieldObject_ViewPoint*>(obj.get())){
            // スタートオブジェクトが見つかったら位置を返す
            return viewPoint;
        }
    }

    return nullptr;
}

////////////////////////////////////////////////////////////////////////
// stageの読み込み
////////////////////////////////////////////////////////////////////////
void Stage::LoadFromJson(const std::string& filePath){
    namespace fs = std::filesystem;

    if(!fs::exists(filePath)){
        std::cerr << "Error: File does not exist: " << filePath << std::endl;
        return;
    }
    std::ifstream file(filePath);
    if(!file.is_open()){
        std::cerr << "Error: Failed to open file: " << filePath << std::endl;
        return;
    }

    nlohmann::json jsonData;
    try{
        file >> jsonData;
    } catch(const nlohmann::json::parse_error& e){
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        file.close();
        return;
    }
    file.close();

    // JSON から "stage" を読み取り
    int32_t stageNo = 0;
    if(jsonData.contains("stage")){
        if(jsonData["stage"].is_number_integer()){
            stageNo = jsonData["stage"].get<int32_t>();
        } else{
            std::cerr << "Warning: 'stage' is not an integer. Using default value 0." << std::endl;
        }
    }

    // いったん全消去
    ClearAllFieldObjects();

    // スイッチと関連ドアIDの一時保存用
    std::vector<std::tuple<FieldObject_Switch*,std::vector<std::string>>> switchDoorAssociations;

    // JSON から "models" 配列を読み取り
    if(jsonData.contains("models") && jsonData["models"].is_array()){
        for(auto& modelJson : jsonData["models"]){
            // 必要なフィールドの型チェック
            std::string name = modelJson.value("name","default_model.obj");
            uint32_t type = 0;
            Vector3 position{0.f,0.f,0.f};
            Vector3 scale{1.f,1.f,1.f};
            Vector3 rotation{0.f,0.f,0.f};

            // position
            if(modelJson.contains("position") && modelJson["position"].is_array() && modelJson["position"].size() >= 3){
                try{
                    position.x = modelJson["position"][0].get<float>();
                    position.y = modelJson["position"][1].get<float>();
                    position.z = modelJson["position"][2].get<float>();
                } catch(const nlohmann::json::type_error& e){
                    std::cerr << "Type Error in 'position': " << e.what() << std::endl;
                }
            }

            // scale
            if(modelJson.contains("scale") && modelJson["scale"].is_array() && modelJson["scale"].size() >= 3){
                try{
                    scale.x = modelJson["scale"][0].get<float>();
                    scale.y = modelJson["scale"][1].get<float>();
                    scale.z = modelJson["scale"][2].get<float>();
                } catch(const nlohmann::json::type_error& e){
                    std::cerr << "Type Error in 'scale': " << e.what() << std::endl;
                }
            }

            // rotation
            if(modelJson.contains("rotation") && modelJson["rotation"].is_array() && modelJson["rotation"].size() >= 3){
                try{
                    rotation.x = modelJson["rotation"][0].get<float>();
                    rotation.y = modelJson["rotation"][1].get<float>();
                    rotation.z = modelJson["rotation"][2].get<float>();
                } catch(const nlohmann::json::type_error& e){
                    std::cerr << "Type Error in 'rotation': " << e.what() << std::endl;
                }
            }

            // type
            if(modelJson.contains("type")){
                if(modelJson["type"].is_number_integer()){
                    type = modelJson["type"].get<uint32_t>();
                } else{
                    std::cerr << "Warning: 'type' is not an integer. Using default value 0." << std::endl;
                }
            }

            // 取得した情報からモデルを追加
            AddModel(type,modelJson,scale,rotation,position);

            // 新規追加されたオブジェクトを取得（最後に追加されたものを想定）
            if(fieldObjects_.empty()){
                std::cerr << "Warning: fieldObjects_ is empty after AddModel." << std::endl;
                continue;
            }
            FieldObject* newObj = fieldObjects_.back().get();

            // objectID の処理
            if(modelJson.contains("fieldObjectID")){
                if(modelJson["fieldObjectID"].is_string()){
                    try{
                        std::string guid = modelJson["fieldObjectID"].get<std::string>();
                        newObj->SetGUID(guid);
                    } catch(const nlohmann::json::type_error& e){
                        std::cerr << "Type Error when setting 'objectID': " << e.what() << std::endl;
                    }
                } else{
                    std::cerr << "Warning: 'objectID' is not a string. Skipping SetGUID." << std::endl;
                }
            }

            // スイッチの場合、関連ドア情報を一時保存
            if(auto* sw = dynamic_cast<FieldObject_Switch*>(newObj)){
                if(modelJson.contains("associatedDoors")){
                    if(modelJson["associatedDoors"].is_array()){
                        std::vector<std::string> doorIDs;
                        bool valid = true;
                        for(auto& doorID : modelJson["associatedDoors"]){
                            if(doorID.is_string()){
                                doorIDs.emplace_back(doorID.get<std::string>());
                            } else{
                                std::cerr << "Warning: 'associatedDoors' contains non-string element. Skipping this doorID." << std::endl;
                                valid = false;
                                break;
                            }
                        }
                        if(valid){
                            switchDoorAssociations.emplace_back(sw,doorIDs);
                        }
                    } else{
                        std::cerr << "Warning: 'associatedDoors' is not an array. Skipping associatedDoors." << std::endl;
                    }
                }
            }
            // 移動する床の場合、ルーチン名を設定
            else if(auto* moveFloor = dynamic_cast<FieldObject_MoveFloor*>(newObj)){
                if(modelJson.contains("routineName")){
                    if(modelJson["routineName"].is_string()){
                        try{
                            moveFloor->SetRoutineName(modelJson["routineName"].get<std::string>());
                        } catch(const nlohmann::json::type_error& e){
                            std::cerr << "Type Error when setting 'routineName': " << e.what() << std::endl;
                        }
                    } else{
                        std::cerr << "Warning: 'routineName' is not a string. Skipping SetRoutineName." << std::endl;
                    }
                }
                if(modelJson.contains("moveSpeed")){
                    if(modelJson["moveSpeed"].is_number()){
                        try{
                            moveFloor->SetMoveSpeed(modelJson["moveSpeed"].get<float>());
                        } catch(const nlohmann::json::type_error& e){
                            std::cerr << "Type Error when setting 'moveSpeed': " << e.what() << std::endl;
                        }
                    } else{
                        std::cerr << "Warning: 'moveSpeed' is not a number. Skipping SetMoveSpeed." << std::endl;
                    }
                }
                moveFloor->InitializeRoutine();
            }
        }
    } else{
        std::cerr << "Warning: 'models' key is missing or not an array in JSON data." << std::endl;
    }

    // 全てのモデルを生成・登録後に、スイッチとドアの関連付けを行う
    for(auto& [sw,doorIDs] : switchDoorAssociations){
        for(const std::string& doorID : doorIDs){
            // manager_ から対応するドアを検索
            std::vector<FieldObject_Door*> doors = GetObjectsOfType<FieldObject_Door>();
            bool doorFound = false;
            for(auto& door : doors){
                if(door && door->GetGUID() == doorID){
                    // スイッチにドアを追加し、ドア側でスイッチをセット
                    sw->AddAssociatedDoor(door);
                    door->SetSwitch(sw);
                    // 1つのドアにつき1回で十分と仮定
                    doorFound = true;
                    break;
                }
            }
            if(!doorFound){
                std::cerr << "Warning: Door with GUID '" << doorID << "' not found." << std::endl;
            }
        }
    }
}



////////////////////////////////////////////////////////////////////////
// 新しいオブジェクトを追加
////////////////////////////////////////////////////////////////////////
void Stage::AddModel(
    uint32_t modelNameIndex,
    const nlohmann::json& json,
    const Vector3& scale,
    const Vector3& rotate,
    const Vector3& translate
){
    // スタートもしくはゴールの場合、既に存在しているかチェックし、
    // 存在していれば新規追加をキャンセルする
    if(modelNameIndex == FIELDMODEL_START || modelNameIndex == FIELDMODEL_GOAL){
        for(const auto& objPtr : fieldObjects_){
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
        case FIELDMODEL_STAR:
            newObj = std::make_unique<FieldObject_Star>();
            // Star を 保持
            starObjects_.push_back(dynamic_cast<FieldObject_Star*>(newObj.get()));
            break;
        case FIELDMODEL_DOOR:
            newObj = std::make_unique<FieldObject_Door>();
            break;
        case FIELDMODEL_START:
            newObj = std::make_unique<FieldObject_Start>();
            // start を 保持
            startObject_ = dynamic_cast<FieldObject_Start*>(newObj.get());
            break;
        case FIELDMODEL_GOAL:
            newObj = std::make_unique<FieldObject_Goal>();
            // goal を 保持
            goalObject_ = dynamic_cast<FieldObject_Goal*>(newObj.get());
            break;
        case FIELDMODEL_SWITCH:
            newObj = std::make_unique<FieldObject_Switch>();
            break;
        case FIELDMODEL_VIEWPOINT:
            newObj = std::make_unique<FieldObject_ViewPoint>();
            break;
        case FIELDMODEL_MOVEFLOOR:
            newObj = std::make_unique<FieldObject_MoveFloor>(routineManager_);
            break;
        case FIELDMODEL_EVENTAREA:
            newObj = std::make_unique<FieldObject_EventArea>();
            if(json.contains("eventFunctionKey")){
                FieldObject_EventArea* eventArea = dynamic_cast<FieldObject_EventArea*>(newObj.get());
                eventArea->SetEvent(EventFunctionTable::tableMap_[json["eventFunctionKey"]]);
                eventArea->SetEventName(json["eventFunctionKey"]);
            }

            if(json.contains("isOnceEvent")){
                FieldObject_EventArea* eventArea = dynamic_cast<FieldObject_EventArea*>(newObj.get());
                eventArea->isOnceEvent_ = json["isOnceEvent"];
            }

            break;
        default:
            break;
    }

    if(!newObj) return;  // newObj が生成されなかった場合は何もしない

    // スタートまたはゴールの場合、スケールを 1 に固定（あるいは別途調整）
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

    // いったん行列更新
    newObj->UpdateMatrix();

    // ◆ドアの場合のみ、「初期Y」を覚えてもらう
    if(modelNameIndex == FIELDMODEL_DOOR){
        auto doorObj = dynamic_cast<FieldObject_Door*>(newObj.get());
        if(doorObj){
            doorObj->SetClosedPosY(translate.y);
        }
    } else if(modelNameIndex == FIELDMODEL_GOAL){ // ゴールの場合、Y座標を上にずらす
        auto goalObj = dynamic_cast<FieldObject_Goal*>(newObj.get());
        if(goalObj){
            goalObj->SetTranslateY(goalObj->GetLocalTranslate().y + 1.0f);
        }
    }
    // Manager に登録
    AddFieldObject(std::move(newObj));
}

void Stage::UpdateStarCount(){
    // 現在の星の数をカウント
    uint32_t starCount = 0;
    for(const auto& star : starObjects_){
        // 取得した星は nullptr になるので、nullptr のものをカウント
        if(!star || star->GetRemoveFlag()){
            starCount++;
        }
    }
    currentStarCount_ = starCount;
}
