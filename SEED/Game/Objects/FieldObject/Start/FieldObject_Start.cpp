#include "FieldObject_Start.h"


uint32_t FieldObject_Start::nextFieldObjectID_ = 1;

////////////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////////////
FieldObject_Start::FieldObject_Start(){ // 名前の初期化
    className_ = "FieldObject_Start";
    name_ = "start";
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

FieldObject_Start::FieldObject_Start(const std::string& modelName)
    : FieldObject(modelName){
    // クラス名の初期化
    className_ = "FieldObject_Start";
    name_ = "start";
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Field);
    // 全般の初期化
    FieldObject::Initialize();
}

////////////////////////////////////////////////////////////////////////
// 初期化関数
////////////////////////////////////////////////////////////////////////
void FieldObject_Start::Initialize(){}

////////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////////
void FieldObject_Start::Update(){
    FieldObject::Update();
}

