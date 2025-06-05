#include "BaseObject.h"

//////////////////////////////////////////////////////////////////////////
// static変数
//////////////////////////////////////////////////////////////////////////
uint32_t BaseObject::nextID_ = 0;



//////////////////////////////////////////////////////////////////////////
// コンストラクタ ・ デストラクタ
//////////////////////////////////////////////////////////////////////////
BaseObject::BaseObject(){
    objectID_ = nextID_++;
    className_ = "BaseObject";
    name_ = "BaseObject";
    Initialize();
}

BaseObject::BaseObject(const std::string& modelFilePath){
    objectID_ = nextID_++;
    className_ = "BaseObject";
    name_ = "BaseObject";
    model_ = std::make_unique<Model>(modelFilePath);
    model_->UpdateMatrix();
}

BaseObject::~BaseObject(){}


//////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////
void BaseObject::Initialize(){
    // モデルの初期化
    model_ = std::make_unique<Model>("Assets/suzanne.obj");
    model_->UpdateMatrix();
    // コライダーの初期化
    InitColliders(ObjectType::Editor);
}



//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void BaseObject::Update(){
    MoveByVelocity();
    // モデルの更新
    model_->Update();
}



//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void BaseObject::Draw(){
    model_->Draw();
}



//////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////////////
void BaseObject::BeginFrame(){

    // 落下フラグの初期化
    isDrop_ = true;

    // 座標の保存
    prePos_ = GetWorldTranslate();

    // コライダーの開始処理
    for(auto& collider : colliders_){
        collider->BeginFrame();
    }
}



//////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
//////////////////////////////////////////////////////////////////////////
void BaseObject::EndFrame(){

    // 落下処理の更新
    EndFrameDropFlagUpdate();

    // コライダーの更新
    EraseCheckColliders();
}


//////////////////////////////////////////////////////////////////////////
// マトリックスの更新
//////////////////////////////////////////////////////////////////////////
void BaseObject::UpdateMatrix(){
    model_->UpdateMatrix();
}

//////////////////////////////////////////////////////////////////////////
// コライダーの編集
//////////////////////////////////////////////////////////////////////////
void BaseObject::EditCollider(){
    if(colliderEditor_){
        colliderEditor_->Edit();
    }
}

void BaseObject::SetCollidable(bool _collidable){
    isHandOverColliders_ = _collidable;
}


// フレーム終了時の落下更新処理
void BaseObject::EndFrameDropFlagUpdate(){
    // 落下フラグの更新
    if(GetWorldTranslate().y <= 0.0f){
        isDrop_ = false;
    }

    // 終了時の落下フラグに応じた処理
    if(isDrop_ && isApplyGravity_){
        float downAccel = -Physics::kGravity * weight_ * ClockManager::DeltaTime();
        dropSpeed_ += downAccel;
        velocity_.y = dropSpeed_;
    } else{
        dropSpeed_ = 0.0f;
        velocity_.y = 0.0f;
    }
}

void BaseObject::MoveByVelocity(){
    model_->transform_.translate += velocity_ * ClockManager::DeltaTime();
}

//////////////////////////////////////////////////////////////////////////
// 親子付けとかされてても常にワールド軸基準で指定した方向に移動を追加する関数
//////////////////////////////////////////////////////////////////////////
void BaseObject::AddWorldTranslate(const Vector3& addValue){
    if(GetParent() != nullptr){
        Matrix4x4 invParentMat = InverseMatrix(RotateMatrix(GetParent()->GetWorldRotate()));
        Vector3 localAddValue = addValue * invParentMat;
        model_->transform_.translate += localAddValue;
    } else{
        model_->transform_.translate += addValue;
    }
}


//////////////////////////////////////////////////////////////////////////
// コライダーの追加
//////////////////////////////////////////////////////////////////////////
void BaseObject::AddCollider(Collider* collider){
    colliders_.emplace_back(std::make_unique<Collider>());
    colliders_.back().reset(collider);
}



//////////////////////////////////////////////////////////////////////////
// コライダーのリセット
//////////////////////////////////////////////////////////////////////////
void BaseObject::ResetCollider(){
    colliders_.clear();
}



//////////////////////////////////////////////////////////////////////////
// コライダーをCollisionManagerに渡す
//////////////////////////////////////////////////////////////////////////
void BaseObject::HandOverColliders(){

    // 衝突情報の保存・初期化
    preIsCollide_ = isCollide_;
    isCollide_ = false;

    if(!isHandOverColliders_){
        return;
    }
    // キャラクターの基本コライダーを渡す
    for(auto& collider : colliders_){
        CollisionManager::AddCollider(collider.get());
    }
}


//////////////////////////////////////////////////////////////////////////
// 衝突処理
//////////////////////////////////////////////////////////////////////////  
void BaseObject::OnCollision( BaseObject* other, ObjectType objectType){
    isCollide_ = true;
    other;
    objectType;
}

//////////////////////////////////////////////////////////////////////////
// 前フレームのコライダーの破棄
//////////////////////////////////////////////////////////////////////////
void BaseObject::DiscardPreCollider(){
    for(auto& collider : colliders_){
        collider->DiscardPreCollider();
    }
}

//////////////////////////////////////////////////////////////////////////
// コライダーの判定スキップリストの追加
//////////////////////////////////////////////////////////////////////////
void BaseObject::AddSkipPushBackType(ObjectType skipType){
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(skipType);
    }
}


//////////////////////////////////////////////////////////////////////////
// コライダーの読み込み
//////////////////////////////////////////////////////////////////////////
void BaseObject::LoadColliders(ObjectType objectType){
    // コライダーの読み込み
    ColliderEditor::LoadColliders(className_ + ".json", this, &colliders_);

    // オブジェクトの属性を取得
    for(auto& collider : colliders_){
        collider->SetObjectType(objectType);
    }
}

void BaseObject::LoadColliders(const std::string& fileName, ObjectType objectType){
    // コライダーの読み込み
    ColliderEditor::LoadColliders(fileName, this, &colliders_);
    // オブジェクトの属性を取得
    for(auto& collider : colliders_){
        collider->SetObjectType(objectType);
    }
}



//////////////////////////////////////////////////////////////////////////
// コライダーの初期化
//////////////////////////////////////////////////////////////////////////
void BaseObject::InitColliders(ObjectType objectType){
    colliders_.clear();
    LoadColliders(objectType);
}

void BaseObject::InitColliders(const std::string& fileName, ObjectType objectType){
    colliders_.clear();
    LoadColliders(fileName, objectType);
}



//////////////////////////////////////////////////////////////////////////
// コライダーの更新
//////////////////////////////////////////////////////////////////////////
void BaseObject::EraseCheckColliders(){

    // 終了した要素の削除
    for(int i = 0; i < colliders_.size(); ++i){
        if(colliders_[i]->IsEndAnimation()){
            colliders_.erase(colliders_.begin() + i);
            --i;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// jsonデータの取得
//////////////////////////////////////////////////////////////////////////
const nlohmann::json& BaseObject::GetJsonData() {
    static nlohmann::json json;
    json["Translate"] = GetWorldTranslate();
    json["Rotate"] = GetWorldRotate();
    json["Scale"] = GetWorldScale();
    return json;
}