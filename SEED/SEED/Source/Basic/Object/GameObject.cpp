#include "GameObject.h"

//////////////////////////////////////////////////////////////////////////
// static変数
//////////////////////////////////////////////////////////////////////////
uint32_t GameObject::nextID_ = 0;



//////////////////////////////////////////////////////////////////////////
// コンストラクタ ・ デストラクタ
//////////////////////////////////////////////////////////////////////////
GameObject::GameObject(){
    objectID_ = nextID_++;
    idolName_ = "BaseObject";
    objectName_ = "BaseObject";
    Initialize();
}

// モデル名指定がある場合
GameObject::GameObject(const std::string& modelFilePath, const std::string& tagName){
    objectID_ = nextID_++;
    idolName_ = "BaseObject";
    objectName_ = "BaseObject";
    // モデルの初期化
    AddComponent<ModelRenderComponent>(tagName)->Initialize(modelFilePath);
}

GameObject::~GameObject(){}


//////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////
void GameObject::Initialize(){
    // コライダーの初期化
    InitColliders(ObjectType::Editor);
}



//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void GameObject::Update(){

    // velocityで移動
    MoveByVelocity();
    // マトリックスの更新
    UpdateMatrix();

    // コンポーネントの更新
    for(auto& component : components_){
        component->Update();
    }

}



//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void GameObject::Draw(){
    // コンポーネントの描画
    for(auto& component : components_){
        component->Draw();
    }
}



//////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////////////
void GameObject::BeginFrame(){

    // 落下フラグの初期化
    isDrop_ = true;

    // 座標の保存
    prePos_ = GetWorldTranslate();

    // コライダーの開始処理
    for(auto& collider : colliders_){
        collider->BeginFrame();
    }

    // コンポーネントの開始処理
    for(auto& component : components_){
        component->BeginFrame();
    }
}



//////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
//////////////////////////////////////////////////////////////////////////
void GameObject::EndFrame(){

    // 落下処理の更新
    EndFrameDropFlagUpdate();

    // コライダーの更新
    EraseCheckColliders();

    // コンポーネントの終了処理
    for(auto& component : components_){
        component->EndFrame();
    }
}


//////////////////////////////////////////////////////////////////////////
// マトリックスの更新
//////////////////////////////////////////////////////////////////////////
void GameObject::UpdateMatrix(){

    // ローカル変換行列の更新
    if(isRotateWithQuaternion_){
        localMat_ = AffineMatrix(transform_.scale, transform_.rotateQuat, transform_.translate);
        transform_.rotate = Quaternion::ToEuler(transform_.rotateQuat);// 切り替えても大丈夫なように同期させておく
    } else{
        localMat_ = AffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
        transform_.rotateQuat = Quaternion::ToQuaternion(transform_.rotate);// 切り替えても大丈夫なように同期させておく
    }

    // ワールド行列の更新
    if(parent_){

        Matrix4x4 parentMat = parent_->worldMat_;

        if(isParentRotate_ + isParentScale_ + isParentTranslate_ == 3){
            worldMat_ = localMat_ * (parentMat);
            return;
        } else{

            Matrix4x4 cancelMat = IdentityMat4();

            // 親の行列から取り出した要素を打ち消す行列を作成
            if(!isParentScale_){
                Vector3 inverseScale = Vector3(1.0f, 1.0f, 1.0f) / ExtractScale(parentMat);
                cancelMat = cancelMat * ScaleMatrix(inverseScale);
            }

            if(!isParentRotate_){
                Vector3 inverseRotate = ExtractRotation(parentMat) * -1.0f;
                cancelMat = cancelMat * RotateMatrix(inverseRotate);
            }

            if(!isParentTranslate_){
                Vector3 inverseTranslate = ExtractTranslation(parentMat) * -1.0f;
                cancelMat = cancelMat * TranslateMatrix(inverseTranslate);
            }

            Matrix4x4 canceledMat = cancelMat * parentMat;
            //worldMat_ = (localMat_ * parentMat) * cancelMat;
            worldMat_ = localMat_ * canceledMat;

        }

    } else{
        worldMat_ = localMat_;
    }
}

//////////////////////////////////////////////////////////////////////////
// コライダーの編集
//////////////////////////////////////////////////////////////////////////
void GameObject::EditCollider(){
    if(colliderEditor_){
        colliderEditor_->Edit();
    }
}

void GameObject::SetCollidable(bool _collidable){
    isHandOverColliders_ = _collidable;
}


// フレーム終了時の落下更新処理
void GameObject::EndFrameDropFlagUpdate(){
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

void GameObject::MoveByVelocity(){
    transform_.translate += velocity_ * ClockManager::DeltaTime();
}

//////////////////////////////////////////////////////////////////////////
// 親子付けとかされてても常にワールド軸基準で指定した方向に移動を追加する関数
//////////////////////////////////////////////////////////////////////////
void GameObject::AddWorldTranslate(const Vector3& addValue){
    if(parent_ != nullptr){
        Matrix4x4 invParentMat = InverseMatrix(RotateMatrix(parent_->GetWorldRotate()));
        Vector3 localAddValue = addValue * invParentMat;
        transform_.translate += localAddValue;
    } else{
        transform_.translate += addValue;
    }
}


//////////////////////////////////////////////////////////////////////////
// コライダーの追加
//////////////////////////////////////////////////////////////////////////
void GameObject::AddCollider(Collider* collider){
    colliders_.emplace_back(std::make_unique<Collider>());
    colliders_.back().reset(collider);
}



//////////////////////////////////////////////////////////////////////////
// コライダーのリセット
//////////////////////////////////////////////////////////////////////////
void GameObject::ResetCollider(){
    colliders_.clear();
}



//////////////////////////////////////////////////////////////////////////
// コライダーをCollisionManagerに渡す
//////////////////////////////////////////////////////////////////////////
void GameObject::HandOverColliders(){

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
void GameObject::OnCollision( GameObject* other, ObjectType objectType){
    isCollide_ = true;
    other;
    objectType;
}

//////////////////////////////////////////////////////////////////////////
// 前フレームのコライダーの破棄
//////////////////////////////////////////////////////////////////////////
void GameObject::DiscardPreCollider(){
    for(auto& collider : colliders_){
        collider->DiscardPreCollider();
    }
}

//////////////////////////////////////////////////////////////////////////
// コライダーの判定スキップリストの追加
//////////////////////////////////////////////////////////////////////////
void GameObject::AddSkipPushBackType(ObjectType skipType){
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(skipType);
    }
}


//////////////////////////////////////////////////////////////////////////
// コライダーの読み込み
//////////////////////////////////////////////////////////////////////////
void GameObject::LoadColliders(ObjectType objectType){
    // コライダーの読み込み
    ColliderEditor::LoadColliders(idolName_ + ".json", this, &colliders_);

    // オブジェクトの属性を取得
    for(auto& collider : colliders_){
        collider->SetObjectType(objectType);
    }
}

void GameObject::LoadColliders(const std::string& fileName, ObjectType objectType){
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
void GameObject::InitColliders(ObjectType objectType){
    colliders_.clear();
    LoadColliders(objectType);
}

void GameObject::InitColliders(const std::string& fileName, ObjectType objectType){
    colliders_.clear();
    LoadColliders(fileName, objectType);
}



//////////////////////////////////////////////////////////////////////////
// コライダーの更新
//////////////////////////////////////////////////////////////////////////
void GameObject::EraseCheckColliders(){

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
const nlohmann::json& GameObject::GetJsonData() {
    static nlohmann::json json;
    json["Translate"] = GetWorldTranslate();
    json["Rotate"] = GetWorldRotate();
    json["Scale"] = GetWorldScale();
    return json;
}