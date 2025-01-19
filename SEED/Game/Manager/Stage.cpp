#include "Stage.h"
#include "CollisionManaer/CollisionManager.h"

//lib
#include <nlohmann/json.hpp>
#include <fstream>

////////////////////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////////////////////
void Stage::Update(){
    for(auto& fieldObject : fieldObjects_){
        // 通常のオブジェクト
        fieldObject->Update();
        fieldObject->EditCollider();
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

void Stage::RemoveFieldObject(FieldObject* obj){
    // オブジェクトが IObserver を実装している場合、EventManager から登録解除
    IObserver* observer = dynamic_cast< IObserver* >(obj);
    if (observer){
        subject_.UnregisterObserver(observer);
    }
    // オブジェクトを削除
    fieldObjects_.erase(
        std::remove_if(
        fieldObjects_.begin(),
        fieldObjects_.end(),
        [obj] (const std::unique_ptr<FieldObject>& fieldObject){
            return fieldObject.get() == obj;
        }
    ),
        fieldObjects_.end()
    );
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
    for(const auto& obj : fieldObjects_){
        // FieldObject_Start 型へのキャストを試みる
        if(auto* start = dynamic_cast<FieldObject_Start*>(obj.get())){
            // スタートオブジェクトが見つかったら位置を返す
            return start->GetWorldTranslate() + Vector3(0.0f,0.3f,0.0f);
        }
    }
    // スタートオブジェクトが見つからなかった場合のデフォルト値を返す
    return Vector3{ 0.0f, 0.0f, 0.0f };
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
        return;
    }
    std::ifstream file(filePath);
    if(!file.is_open()){
        return;
    }

    nlohmann::json jsonData;
    file >> jsonData;
    file.close();


    // JSON から "stage" を読み取り
    int32_t stageNo = 0;
    if(jsonData.contains("stage")){ stageNo = jsonData["stage"]; }

    // いったん全消去
    ClearAllFieldObjects();

    // スイッチと関連ドアIDの一時保存用
    std::vector<std::tuple<FieldObject_Switch*, std::vector<int>>> switchDoorAssociations;

    // JSON から "models" 配列を読み取り
    if(jsonData.contains("models")){
        for(auto& modelJson : jsonData["models"]){
            std::string name = modelJson.value("name", "default_model.obj");
            uint32_t type = 0;
            Vector3 position{ 0.f, 0.f, 0.f };
            Vector3 scale{ 1.f, 1.f, 1.f };
            Vector3 rotation{ 0.f, 0.f, 0.f };

            if(modelJson.contains("position")){
                position.x = modelJson["position"][0];
                position.y = modelJson["position"][1];
                position.z = modelJson["position"][2];
            }
            if(modelJson.contains("scale")){
                scale.x = modelJson["scale"][0];
                scale.y = modelJson["scale"][1];
                scale.z = modelJson["scale"][2];
            }
            if(modelJson.contains("rotation")){
                rotation.x = modelJson["rotation"][0];
                rotation.y = modelJson["rotation"][1];
                rotation.z = modelJson["rotation"][2];
            }
            if(modelJson.contains("type")){
                type = modelJson["type"];
            }

            // 取得した情報からモデルを追加
            AddModel(type, scale, rotation, position);

            // 新規追加されたオブジェクトを取得（最後に追加されたものを想定
            if(fieldObjects_.empty()) continue;
            FieldObject* newObj = fieldObjects_.back().get();

            // スイッチの場合、関連ドア情報を一時保存
            if(auto* sw = dynamic_cast<FieldObject_Switch*>(newObj)){
                //json から associatedDoors を取得(ドアIDの配列)
                if(modelJson.contains("associatedDoors")){
                    std::vector<int> doorIDs = modelJson["associatedDoors"].get<std::vector<int>>();
                    switchDoorAssociations.emplace_back(sw, doorIDs);
                }
            }
        }
    }

    // 全てのモデルを生成・登録後に、スイッチとドアの関連付けを行う
    for(auto& [sw, doorIDs] : switchDoorAssociations){
        for(uint32_t doorID : doorIDs){
            // manager_ から対応するドアを検索
            std::vector<FieldObject_Door*> doors = GetObjectsOfType<FieldObject_Door>();
            for(auto& door : doors){
                if(door){
                    if(door->GetFieldObjectID() == doorID){
                        // スイッチにドアを追加し、ドア側でスイッチをセット
                        sw->AddAssociatedDoor(door);
                        door->SetSwitch(sw);
                        // 1つのドアにつき1回で十分と仮定
                        break;
                    }
                }
            }
        }
    }
}



////////////////////////////////////////////////////////////////////////
// 新しいオブジェクトを追加
////////////////////////////////////////////////////////////////////////
void Stage::AddModel(
    uint32_t modelNameIndex,
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
    case FIELDMODEL_SPHERE:
        newObj = std::make_unique<FieldObject_Sphere>();
        break;
    case FIELDMODEL_DOOR:
        newObj = std::make_unique<FieldObject_Door>();
        break;
    case FIELDMODEL_START:
        newObj = std::make_unique<FieldObject_Start>();
        break;
    case FIELDMODEL_GOAL:
        newObj = std::make_unique<FieldObject_Goal>();
        break;
    case FIELDMODEL_SWITCH:
        newObj = std::make_unique<FieldObject_Switch>();
        break;
    case FIELDMODEL_VIEWPOINT:
        newObj = std::make_unique<FieldObject_ViewPoint>();
        break;
    default:
        break;
    }

    if(!newObj) return;  // newObj が生成されなかった場合は何もしない

    // スタートまたはゴールの場合、スケールを1/10に調整
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
    newObj->UpdateMatrix();

    // Manager に登録
    AddFieldObject(std::move(newObj));
}
