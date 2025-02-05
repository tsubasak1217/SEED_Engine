#include "Stage.h"
#include "CollisionManaer/CollisionManager.h"
#include "EventState/EventFunctionTable.h"
#include "Player/Player.h"

// PlayerState
#include "Player/PlayerState/PlayerState_Idle.h"

#include "StageManager.h"

// fieldObject
#include "FieldObject/MoveFloor/FieldObject_MoveFloor.h"
#include "FieldObject/Lever/FieldObject_Lever.h"
#include "FieldObject/Activator/FieldObject_Activator.h"

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
Stage::Stage(ISubject& subject):subject_(subject){
    // ステージの初期化
    ClearAllFieldObjects();

    //タイトル用に作成したため0
    stageNo_ = 0;

    routineManager_.LoadRoutines(stageNo_);

    enemyManager_ = std::make_unique<EnemyManager>(pPlayer_,stageNo_,routineManager_);

    playerCorpseManager_ = std::make_unique<PlayerCorpseManager>();
    playerCorpseManager_->Initialize();
}

void Stage::InitializeStatus(bool isSaveData){
    std::string filePath;

    if(!isSaveData){
        filePath = "resources/jsons/Stages/stage_" + std::to_string(stageNo_ + 1) + ".json";
    } else{
        filePath = "resources/jsons/StageSaveDatas/stage_" + std::to_string(stageNo_ + 1) + ".json";
    }

    InitializeStatus(filePath);
}

void Stage::InitializeStatus(const std::string& _jsonFilePath){
    //======================== オブジェクトの初期化 ========================//
    // 特殊オブジェクトを nullptr に初期化
    startObject_ = nullptr;
    goalObject_ = nullptr;
    starObjects_.clear();
    currentStarSoundVolume_ = 0.0f;

    // Jsonから読み込み
    LoadFromJson(_jsonFilePath);

    // _jsonFilePathのファイルをコピーする
    std::string copyfilePath = "resources/jsons/StageSaveDatas/stage_" + std::to_string(stageNo_ + 1) + ".json";
    bool isSave = true;
    if(_jsonFilePath != copyfilePath){
        std::ifstream ifs(_jsonFilePath);
        std::ofstream ofs(copyfilePath);
        ofs << ifs.rdbuf();
        ifs.close();
        isSave = false;
    }

    // 敵の初期化
    enemyManager_->LoadEnemies(isSave);

    // player
    {
        pPlayer_->SetScale(Vector3{1.0f,1.0f,1.0f});
        pPlayer_->ChangeState(new PlayerState_Idle("PlayerState_Idle",pPlayer_));
    }

    { // 死体削除
        if(!playerCorpseManager_->GetIsEmpty()){
            playerCorpseManager_->RemoveAll();
        }
    }

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

        // woodだったらコライダーを編集
        if(FieldObject_Wood* wood = dynamic_cast<FieldObject_Wood*>(fieldObject.get())){
            fieldObject->EditCollider();
        }
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

void Stage::DrawHUD(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->DrawHud();
    }
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
    playerCorpseManager_->HandOverColliders();
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

    // JSON から "stage" を読み取り (ステージ番号など)
    int32_t stageNo = 0;
    if(jsonData.contains("stage")){
        if(jsonData["stage"].is_number_integer()){
            stageNo = jsonData["stage"].get<int32_t>();
        } else{
            std::cerr << "Warning: 'stage' is not an integer. Using default 0." << std::endl;
        }
    }

    // いったんこのステージのオブジェクトを全消去
    ClearAllFieldObjects();

    //===============================================================
    // ★★ (A) Activator と Door/Floor の紐付け用の一時保管
    //===============================================================
    // ここでは 「Activatorのポインタ, 紐付くドアGUID配列, 紐付く床GUID配列」 をまとめて保持
    struct ActivatorAssoc{
        FieldObject_Activator* activator;
        std::vector<std::string> doorIDs;
        std::vector<std::string> floorIDs;
    };
    std::vector<ActivatorAssoc> activatorAssociations;

    // JSON から "models" 配列を読み取り
    if(jsonData.contains("models") && jsonData["models"].is_array()){
        for(auto& modelJson : jsonData["models"]){
            // 各フィールド読み取り
            std::string name = modelJson.value("name","default_model.obj");
            uint32_t type = modelJson.value("type",0U);

            // ポジション / スケール / 回転
            Vector3 position{0.f,0.f,0.f};
            Vector3 scale{1.f,1.f,1.f};
            Vector3 rotation{0.f,0.f,0.f};

            // position
            if(modelJson.contains("position") && modelJson["position"].is_array()){
                auto arr = modelJson["position"];
                if(arr.size() >= 3){
                    position.x = arr[0].get<float>();
                    position.y = arr[1].get<float>();
                    position.z = arr[2].get<float>();
                }
            }
            // scale
            if(modelJson.contains("scale") && modelJson["scale"].is_array()){
                auto arr = modelJson["scale"];
                if(arr.size() >= 3){
                    scale.x = arr[0].get<float>();
                    scale.y = arr[1].get<float>();
                    scale.z = arr[2].get<float>();
                }
            }
            // rotation
            if(modelJson.contains("rotation") && modelJson["rotation"].is_array()){
                auto arr = modelJson["rotation"];
                if(arr.size() >= 3){
                    rotation.x = arr[0].get<float>();
                    rotation.y = arr[1].get<float>();
                    rotation.z = arr[2].get<float>();
                }
            }

            // モデルを生成してステージに追加
            AddModel(type,modelJson,scale,rotation,position);
            if(fieldObjects_.empty()){
                std::cerr << "Warning: fieldObjects_ is empty after AddModel." << std::endl;
                continue;
            }
            // 新たに追加されたオブジェクト
            FieldObject* newObj = fieldObjects_.back().get();

            // GUIDの設定
            if(modelJson.contains("fieldObjectID") && modelJson["fieldObjectID"].is_string()){
                std::string guid = modelJson["fieldObjectID"].get<std::string>();
                newObj->SetGUID(guid);
            }

            //===========================================================
            // ★★ (B) Activatorの場合 → ドア/床との紐付け情報を一時保存
            //===========================================================
            if(auto* activator = dynamic_cast<FieldObject_Activator*>(newObj)){
                ActivatorAssoc assoc;
                assoc.activator = activator;

                // associatedDoors
                if(modelJson.contains("associatedDoors") && modelJson["associatedDoors"].is_array()){
                    for(auto& doorID : modelJson["associatedDoors"]){
                        if(doorID.is_string()){
                            assoc.doorIDs.push_back(doorID.get<std::string>());
                        }
                    }
                }
                // associatedFloors
                if(modelJson.contains("associatedFloors") && modelJson["associatedFloors"].is_array()){
                    for(auto& floorID : modelJson["associatedFloors"]){
                        if(floorID.is_string()){
                            assoc.floorIDs.push_back(floorID.get<std::string>());
                        }
                    }
                }

                // Switch なら「必要重量」を読み込む
                if(auto* sw = dynamic_cast<FieldObject_Switch*>(activator)){
                    if(modelJson.contains("requiredWeight")){
                        sw->SetRequiredWeight(modelJson["requiredWeight"].get<int>());
                    } else{
                        sw->SetRequiredWeight(1);
                    }
                }
                // レバーなら他にパラメータがあれば読み込む
                //  (例: if(auto* lv = dynamic_cast<FieldObject_Lever*>(activator)) { ... })

                // 最後に vector に追加
                activatorAssociations.push_back(assoc);
            }
            //===========================================================
            // ★★ (C) MoveFloor
            //===========================================================
            else if(auto* moveFloor = dynamic_cast<FieldObject_MoveFloor*>(newObj)){
                if(modelJson.contains("routineName") && modelJson["routineName"].is_string()){
                    moveFloor->SetRoutineName(modelJson["routineName"].get<std::string>());
                }
                if(modelJson.contains("moveSpeed") && modelJson["moveSpeed"].is_number()){
                    moveFloor->SetMoveSpeed(modelJson["moveSpeed"].get<float>());
                }
                moveFloor->InitializeRoutine();
            }
        }
    } else{
        std::cerr << "Warning: 'models' key is missing or not an array in JSON data." << std::endl;
    }

    //===============================================================
    // (D) Activator と Door/MoveFloor を再リンク
    //===============================================================
    for(auto& assoc : activatorAssociations){
        auto* activator = assoc.activator;
        // 1) Door を GUID で探してリンク
        for(const auto& doorID : assoc.doorIDs){
            auto doors = GetObjectsOfType<FieldObject_Door>();
            for(auto* door : doors){
                if(door && door->GetGUID() == doorID){
                    // Activator 側に door を登録
                    activator->AddAssociatedDoor(door);
                    door->SetActivator(activator);
                }
            }
        }
        // 2) MoveFloor を GUID で探してリンク
        for(const auto& floorID : assoc.floorIDs){
            auto floors = GetObjectsOfType<FieldObject_MoveFloor>();
            for(auto* mf : floors){
                if(mf && mf->GetGUID() == floorID){
                    activator->AddAssociatedMoveFloor(mf);
                    mf->SetActivator(activator);
                }
            }
        }
    }

    // プレイヤーの情報があるとき
    if(jsonData.contains("player")){
        pPlayer_->SetTranslate(jsonData["player"]["Translate"]);
        pPlayer_->SetRotate(Vector3(jsonData["player"]["Rotate"]));
        pPlayer_->SetScale(jsonData["player"]["Scale"]);
        pPlayer_->SetGrowLevel(jsonData["player"]["GrowLevel"]);
        pPlayer_->SetIsDrop(false);
        pPlayer_->SetDropSpeed(0.0f);
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

            if(json.contains("openSpeed")){
                FieldObject_Door* door = dynamic_cast<FieldObject_Door*>(newObj.get());
                door->SetOpenSpeed(json["openSpeed"]);
            }

            if(json.contains("openHeight")){
                FieldObject_Door* door = dynamic_cast<FieldObject_Door*>(newObj.get());
                door->SetOpenHeight(json["openHeight"]);
            }

            if(json.contains("closedPosY")){
                FieldObject_Door* door = dynamic_cast<FieldObject_Door*>(newObj.get());
                door->SetClosedPosY(json["closedPosY"]);
            }
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
        case FIELDMODEL_LEVER:
            newObj = std::make_unique<FieldObject_Lever>();
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

        case FIELDMODEL_POINTLIGHT:
            newObj = std::make_unique<FieldObject_PointLight>();

            if(json.contains("color")){
                FieldObject_PointLight* pointLight = dynamic_cast<FieldObject_PointLight*>(newObj.get());
                pointLight->pointLight_->color_ = json["color"];
            }

            if(json.contains("intensity")){
                FieldObject_PointLight* pointLight = dynamic_cast<FieldObject_PointLight*>(newObj.get());
                pointLight->pointLight_->intensity = json["intensity"];
            }

            if(json.contains("radius")){
                FieldObject_PointLight* pointLight = dynamic_cast<FieldObject_PointLight*>(newObj.get());
                pointLight->pointLight_->radius = json["radius"];
            }

            if(json.contains("decay")){
                FieldObject_PointLight* pointLight = dynamic_cast<FieldObject_PointLight*>(newObj.get());
                pointLight->pointLight_->decay = json["decay"];
            }

            break;

        case FIELD_MODEL_PLANT:
            newObj = std::make_unique<FieldObject_Plant>();
            if(json.contains("isBloomFlower")){
                FieldObject_Plant* plant = dynamic_cast<FieldObject_Plant*>(newObj.get());
                plant->isBloomFlower_ = json["isBloomFlower"];
            }
            if(json.contains("flowerVolume")){
                FieldObject_Plant* plant = dynamic_cast<FieldObject_Plant*>(newObj.get());
                plant->flowerVolume_ = json["flowerVolume"];
            }
            if(json.contains("flowerColor")){
                FieldObject_Plant* plant = dynamic_cast<FieldObject_Plant*>(newObj.get());
                plant->flowerColor_ = json["flowerColor"];
            }
            break;

        case FIELDMODEL_WOOD:
            newObj = std::make_unique<FieldObject_Wood>();

            if(json.contains("leafColor")){
                FieldObject_Wood* wood = dynamic_cast<FieldObject_Wood*>(newObj.get());
                wood->leafColor_ = json["leafColor"];
            }

            break;

        case FIELDMODEL_FENCE:
            newObj = std::make_unique<FieldObject_Fence>();
            break;

        case FIELDMODEL_TILE:
            newObj = std::make_unique<FieldObject_Tile>();
            break;

        case FIELDMODEL_BOX:
            newObj = std::make_unique<FieldObject_Box>();
            break;

        case FIELDMODEL_CHIKUWA:
            newObj = std::make_unique<FieldObject_Chikuwa>();
            break;

        case FIELDMODEL_SAVEAREA:
            newObj = std::make_unique<FieldObject_SaveArea>();
            break;

        default:
            break;
    }

    if(!newObj) return;  // newObj が生成されなかった場合は何もしない

    newObj->SetPlayer(pPlayer_);

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
    }
    // Manager に登録
    AddFieldObject(std::move(newObj));
}

void Stage::StarSEUpdate(){
    float nearestDistancePlayerToStar = 100.f;
    for(auto& object : fieldObjects_){
        FieldObject_Star* starObject = dynamic_cast<FieldObject_Star*>(object.get());
        if(!starObject){
            continue;
        }
        float distancePlayerToStar = MyMath::Length(pPlayer_->GetWorldTranslate() - starObject->GetWorldTranslate());
        nearestDistancePlayerToStar = (std::min)(nearestDistancePlayerToStar,distancePlayerToStar);
        float t = nearestDistancePlayerToStar / distanceToPlayStarSound_;
        // 一定以上離れているなら再生しない
        if(t >= 1.f){
            currentStarSoundVolume_ = 0.f;
        } else{
            // 遠いほど音量を下げる
            currentStarSoundVolume_ = minStarSoundVolume_ + (maxStarSoundVolume_ - minStarSoundVolume_) * (1.f - t);
        }
        AudioManager::SetAudioVolume(starSoundFileName_,currentStarSoundVolume_);
    }
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
