#include "FieldObject_EventArea.h"
#include <../GameSystem.h>

uint32_t FieldObject_EventArea::nextFieldObjectID_ = 1;

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
    model_->color_.w = 0.2f;
    model_->blendMode_ = BlendMode::NORMAL;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::EventArea);
    // 全般の初期化
    FieldObject::Initialize();

    fieldObjectID_ = nextFieldObjectID_++;
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

    fieldObjectID_ = nextFieldObjectID_++;
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
void FieldObject_EventArea::OnCollision(const BaseObject* other, ObjectType objectType) {

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
