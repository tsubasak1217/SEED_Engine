#include "Stage.h"
#include "GameState_Play.h"
#include "CollisionManaer/CollisionManager.h"
#include "EventState/EventFunctionTable.h"
#include "../GameSystem.h"
#include "StageManager.h"
#include "Base/BaseCharacter.h"

//lib
#include <nlohmann/json.hpp>
#include <fstream>

Stage::Stage(uint32_t stageNo){
    // ステージの初期化
    ClearAllFieldObjects();

    stageNo_ = stageNo;
}


void Stage::InitializeStatus(bool isSaveData){
    std::string filePath;

    if(!isSaveData){
        filePath = "resources/jsons/Stages/stage_" + std::to_string(stageNo_ + 1) + ".json";
    } else{
        filePath = "resources/jsons/StageSaveDatas/stage_" + std::to_string(stageNo_ + 1) + ".json";
    }

    InitializeStatus(filePath);
    ParticleManager::DeleteAll();
}

void Stage::InitializeStatus(const std::string& _jsonFilePath){
    //======================== オブジェクトの初期化 ========================//
    // 特殊オブジェクトを nullptr に初期化
    startObject_ = nullptr;
    goalObject_ = nullptr;

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



    //======================== その他 ========================//

    //カメラがオブジェクトをviewしているモードの時はプレイヤーを動かさない
    if(FollowCamera* camera = dynamic_cast<FollowCamera*>(CameraManager::GetActiveCamera())){
        if(camera->GetIsViewingObject()){
            camera->SetisViewingObject(false);
        }
    }
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

    //カメラがオブジェクトをviewしているモードの時はプレイヤーを動かさない
    if(FollowCamera* camera = dynamic_cast<FollowCamera*>(CameraManager::GetActiveCamera())){

        if(camera->GetIsViewingObject()){
            camera->SetPhi(MyMath::Deg2Rad(80.0f));
            //pPlayer_->SetIsMovable(false);
        } else{
            if(!GameSystem::GetScene()->HasEvent()){
                if(GameState_Play* playState = dynamic_cast<GameState_Play*>(GameSystem::GetScene()->GetCurrentState())){
                    //pPlayer_->SetIsMovable(true);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////
// 描画関数
////////////////////////////////////////////////////////////////////////
void Stage::Draw(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->Draw();
    }
}


////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
////////////////////////////////////////////////////////////////////////
void Stage::BeginFrame(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->BeginFrame();
    }
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
}

////////////////////////////////////////////////////////////////////////
// すべてのオブジェクトをクリア
////////////////////////////////////////////////////////////////////////
void Stage::ClearAllFieldObjects(){

    fieldObjects_.clear();
    startObject_ = nullptr;
    goalObject_ = nullptr;
}

////////////////////////////////////////////////////////////////////////
// 新しいオブジェクトを追加
////////////////////////////////////////////////////////////////////////
void Stage::AddFieldObject(std::unique_ptr<FieldObject> obj){

    fieldObjects_.push_back(std::move(obj));

}

void Stage::RemoveFieldObject(FieldObject* objToRemove){
    auto it = std::remove_if(fieldObjects_.begin(), fieldObjects_.end(),
        [&](const std::unique_ptr<FieldObject>& objPtr){
        return objPtr.get() == objToRemove;
    });
    if(it != fieldObjects_.end()){
        // 選択オブジェクトが削除対象の場合、選択をクリア
        if(selectedObjectGUID_ == objToRemove->GetGUID()){
            selectedObjectGUID_.clear();
        }
        fieldObjects_.erase(it, fieldObjects_.end());
    }
}

////////////////////////////////////////////////////////////////////////
// CollisionManagerにコライダーを渡す
////////////////////////////////////////////////////////////////////////
void Stage::HandOverColliders(){
    for(auto& fieldObject : fieldObjects_){
        fieldObject->HandOverColliders();
    }
}

Vector3 Stage::GetStartPosition() const{
    if(startObject_){
        return startObject_->GetWorldTranslate() + Vector3{ 0.0f,1.0f,0.0f };
    }
    // スタートオブジェクトが見つからなかった場合のデフォルト値を返す
    return Vector3{ 0.0f,0.0f,0.0f };
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

    // JSON から "models" 配列を読み取り
    if(jsonData.contains("models") && jsonData["models"].is_array()){
        for(auto& modelJson : jsonData["models"]){
            // 各フィールド読み取り
            uint32_t type = modelJson.value("type", 0U);

            // モデルを生成してステージに追加
            AddModel(type, modelJson);
        }

    } else{
        std::cerr << "Warning: 'models' key is missing or not an array in JSON data." << std::endl;
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
    case FIELDMODEL_VIEWPOINT:
        newObj = std::make_unique<FieldObject_ViewPoint>();
        break;
    case FIELDMODEL_EVENTAREA:
        newObj = std::make_unique<FieldObject_EventArea>();
        break;

    case FIELDMODEL_POINTLIGHT:
        newObj = std::make_unique<FieldObject_PointLight>();
        break;

    case FIELD_MODEL_PLANT:
        newObj = std::make_unique<FieldObject_Plant>();
        break;

    case FIELDMODEL_WOOD:
        newObj = std::make_unique<FieldObject_Wood>();

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

    case FIELDMODEL_CAMERACONTROLAREA:
        newObj = std::make_unique<FieldObject_CameraControlArea>();
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

    // オブジェクトタイプの設定
    newObj->SetFieldObjectType(modelNameIndex);

    // 初期値の設定
    if(!json.empty()){
        // JSON から読み込み
        newObj->LoadFromJson(json);

    } else{
        newObj->SetScale(adjustedScale);
        newObj->SetRotate(rotate);
        newObj->SetTranslate(translate);
    }

    // いったん行列更新
    newObj->UpdateMatrix();

    // Manager に登録
    AddFieldObject(std::move(newObj));
}