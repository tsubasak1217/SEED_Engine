#include "CollisionComponent.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ
//////////////////////////////////////////////////////////////////////////
CollisionComponent::CollisionComponent(GameObject* pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){

    if(tagName == ""){
        componentTag_ = "ModelRender_ID:" + std::to_string(componentID_);
    }

#ifdef _DEBUG
    // コライダーエディターの初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(owner_->GetName(), owner_);
#endif // _DEBUG
}

//////////////////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::Initialize(const std::string& fileName, ObjectType objectType){
    // コライダーの初期化
    InitColliders(fileName, objectType);
}


//////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::BeginFrame(){
    // コライダーの開始処理
    for(auto& collider : colliders_){
        collider->BeginFrame();
    }

    // アクティブな場合のみコライダーを渡す
    if(isActive_){
        HandOverColliders();

    #ifdef _DEBUG
        colliderEditor_->HandOverColliders();
    #endif // _DEBUG
    }
}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::Update(){
    // コライダーの更新
    for(auto& collider : colliders_){
        collider->Update();
    }
}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::Draw(){
    // コライダーの描画
    if(isColliderVisible_){
        for(auto& collider : colliders_){
            collider->Draw();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::EndFrame(){
    // コライダーの更新
    EraseCheckColliders();
    // コライダーをCollisionManagerに渡す
    HandOverColliders();
}


//////////////////////////////////////////////////////////////////////////
// 終了処理
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::Finalize(){
}

//////////////////////////////////////////////////////////////////////////
// GUI編集
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::EditGUI(){
#ifdef _DEBUG
    ImGui::Indent();

    // コライダーエディターのGUI編集
    colliderEditor_->Edit();

    ImGui::Unindent();
#endif
}


//////////////////////////////////////////////////////////////////////////
// コライダーの追加
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::AddCollider(Collider* collider){
    colliders_.emplace_back(std::make_unique<Collider>());
    colliders_.back().reset(collider);
}


//////////////////////////////////////////////////////////////////////////
// コライダーの読み込み
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::LoadColliders(const std::string& fileName, ObjectType objectType){
    // コライダーの読み込み
    ColliderEditor::LoadColliders(fileName, owner_, &colliders_);
    // オブジェクトの属性を取得
    for(auto& collider : colliders_){
        collider->SetObjectType(objectType);
    }
}



//////////////////////////////////////////////////////////////////////////
// コライダーの初期化
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::InitColliders(const std::string& fileName, ObjectType objectType){
    colliders_.clear();
    LoadColliders(fileName, objectType);
}



//////////////////////////////////////////////////////////////////////////
// コライダーの削除チェック
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::EraseCheckColliders(){

    // 終了した要素の削除
    for(int i = 0; i < colliders_.size(); ++i){
        if(colliders_[i]->IsEndAnimation()){
            colliders_.erase(colliders_.begin() + i);
            --i;
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// コライダーのリセット
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::ResetCollider(){
    colliders_.clear();
}


//////////////////////////////////////////////////////////////////////////
// コライダーをCollisionManagerに渡す
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::HandOverColliders(){

    // 衝突情報の保存・初期化
    preIsCollide_ = isCollide_;
    isCollide_ = false;

    if(!isActive_){
        return;
    }
    // キャラクターの基本コライダーを渡す
    for(auto& collider : colliders_){
        CollisionManager::AddCollider(collider.get());
    }
}

//////////////////////////////////////////////////////////////////////////
// コライダーの判定スキップリストの追加
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::AddSkipPushBackType(ObjectType skipType){
    for(auto& collider : colliders_){
        collider->AddSkipPushBackType(skipType);
    }
}


//////////////////////////////////////////////////////////////////////////
// Jsonデータの読み込み
//////////////////////////////////////////////////////////////////////////
void CollisionComponent::LoadFromJson(const nlohmann::json& jsonData){
    jsonData;//あとで定義します
}


//////////////////////////////////////////////////////////////////////////
// Jsonデータの出力
//////////////////////////////////////////////////////////////////////////
nlohmann::json CollisionComponent::GetJsonData() const{
    return nlohmann::json();//あとで定義します
}
