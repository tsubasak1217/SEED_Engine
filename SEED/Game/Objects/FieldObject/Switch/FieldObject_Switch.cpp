#include "FieldObject_Switch.h"

////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////////////
FieldObject_Switch::FieldObject_Switch(){
    // 名前の初期化
    className_ = "FieldObject_Switch";
    name_ = "switch";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();
}

FieldObject_Switch::FieldObject_Switch(const std::string& modelName) : FieldObject(modelName){
    // クラス名の初期化
    className_ = "FieldObject_Switch";
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();

}

////////////////////////////////////////////////////////////////////////
// 初期化関数
////////////////////////////////////////////////////////////////////////
void FieldObject_Switch::Initialize(){}

////////////////////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////////////////////
void FieldObject_Switch::Update(){
    //入力を検知してスイッチの状態をトグル

    FieldObject::Update();
}

////////////////////////////////////////////////////////////////////////
// Observer 用関数
////////////////////////////////////////////////////////////////////////
void FieldObject_Switch::RegisterObserver(IObserver* observer){
    observers_.push_back(observer);
}

void FieldObject_Switch::UnregisterObserver(IObserver* observer){
    observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
}

void FieldObject_Switch::Notify(const std::string& event, void* data){
    for (auto& observer : observers_){
        observer->OnNotify(event, data);
    }
}

////////////////////////////////////////////////////////////////////////
// スイッチの状態をトグルする
////////////////////////////////////////////////////////////////////////
void FieldObject_Switch::Toggle(){
    isActivated_ = !isActivated_;
    std::string event = isActivated_ ? "SwitchActivated" : "SwitchDeactivated";
    Notify(event);  // 登録された Observer に通知
}
