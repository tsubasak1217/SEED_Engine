#include "FieldObject_EventArea.h"
#include <../GameSystem.h>
#include <EventState/EventFunctionTable.h>

/////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
/////////////////////////////////////////////////////////////////////
FieldObject_EventArea::FieldObject_EventArea() {
    // 名前関連の初期化
    className_ = "FieldObject_EventArea";
    name_ = "EventArea";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    model_->meshColor_[0] = { 1.0f,1.0f,0.0f,1.0f };
    model_->color_.w = 0.2f;
    model_->blendMode_ = BlendMode::ADD;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::EventArea);
    // 全般の初期化
    FieldObject::Initialize();
}

FieldObject_EventArea::FieldObject_EventArea(const std::string& modelName) {
    // 名前関連の初期化
    className_ = "FieldObject_EventArea";
    name_ = modelName;
    // モデルの初期化
    std::string path = "FieldObject/" + modelName;
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    model_->color_.w = 0.2f;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::EventArea);
    // 全般の初期化
    FieldObject::Initialize();
}


/////////////////////////////////////////////////////////////////////
// 初期化関数
/////////////////////////////////////////////////////////////////////
void FieldObject_EventArea::Initialize() {}

/////////////////////////////////////////////////////////////////////
// 描画関数
/////////////////////////////////////////////////////////////////////
void FieldObject_EventArea::Draw() {
#ifdef _DEBUG
    model_->color_.w = 0.2f;
    FieldObject::Draw();
#endif // _DEBUG
}

/////////////////////////////////////////////////////////////////////
// フレーム開始処理
/////////////////////////////////////////////////////////////////////
void FieldObject_EventArea::BeginFrame() {
    FieldObject::BeginFrame();
    preIsCollidePlayer_ = isCollidePlayer_;
    isCollidePlayer_ = false;
}

/////////////////////////////////////////////////////////////////////
// 衝突時処理
/////////////////////////////////////////////////////////////////////
void FieldObject_EventArea::OnCollision( BaseObject* other, ObjectType objectType) {

    other;

    if (objectType == ObjectType::Player) {

        isCollidePlayer_ = true;

        // 衝突時
        if (!preIsCollidePlayer_ && isCollidePlayer_) {

            // イベント発生
            if(event_){
                event_(GameSystem::GetScene());
            }

            // 一度だけのイベントなら削除依頼を出す
            if (isOnceEvent_) {
                removeFlag_ = true;
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////
// 編集関数
/////////////////////////////////////////////////////////////////////
void FieldObject_EventArea::Edit(){
#ifdef _DEBUG
    // 基本の編集
    FieldObject::Edit();

    // 固有の編集
    ImGui::Separator();
    ImGui::Text("EventArea Settings");	ImGui::Separator();
    ImGui::Separator();

    // EventFunctionTableの関数名をドロップダウンで表示・選択
    static int selectedEventIndex = 0;
    static std::vector<std::string> eventNames;
    if(eventNames.empty()){
        for(const auto& [name, func] : EventFunctionTable::tableMap_){
            if(name != ""){
                eventNames.push_back(name);
            }
        }
    }

    std::string currentEventName;
    if(eventName_ != ""){
        currentEventName = eventName_;
    } else{
        currentEventName = "none";
    }

    // ドロップダウンリストを作成	
    if(ImGui::BeginCombo("Select Event", currentEventName.c_str())){
        for(int i = 0; i < eventNames.size(); ++i){
            bool isSelected = (selectedEventIndex == i);
            if(ImGui::Selectable(eventNames[i].c_str(), isSelected)){
                selectedEventIndex = i;  // 選択されたイベントのインデックスを更新	
                SetEvent(EventFunctionTable::tableMap_[eventNames[i]]);
                SetEventName(eventNames[i]);
            }
            if(isSelected){
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // 一度のみのイベントかどうかのフラグ	
    ImGui::Checkbox("Is Once Event", &isOnceEvent_);
#endif // _DEBUG
}


/////////////////////////////////////////////////////////////////////
// Json出力関数
/////////////////////////////////////////////////////////////////////
nlohmann::json FieldObject_EventArea::OutputJson(){
    nlohmann::json json = FieldObject::OutputJson();
    json["eventFunctionKey"] = eventName_;
    json["isOnceEvent"] = isOnceEvent_;
    return json;
}
