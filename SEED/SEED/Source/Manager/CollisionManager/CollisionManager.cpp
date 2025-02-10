#include "CollisionManager.h"
#include <SEED/Source/Manager/CollisionManager/Collision.h>

/////////////////////////////////////////////////////////////////////////////////////
// static変数の初期化
/////////////////////////////////////////////////////////////////////////////////////

CollisionManager* CollisionManager::instance_ = nullptr;

/////////////////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・初期化関数
/////////////////////////////////////////////////////////////////////////////////////

CollisionManager::~CollisionManager(){}

CollisionManager* CollisionManager::GetInstance(){
    if(instance_ == nullptr){
        instance_ = new CollisionManager();
    }
    return instance_;
}

void CollisionManager::Initialize(){

    // インスタンスの取得
    GetInstance();

    // 八分木の初期化
    instance_->octree_ = std::make_unique<Octree>(
        AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(2000.0f, 2000.0f, 2000.0f)),
        0, 5, nullptr
    );

    // コライダーリストの初期化
    ResetColliderList();

#ifdef _DEBUG
    instance_->isDrawCollider_ = true;
#endif // _DEBUG

}

/////////////////////////////////////////////////////////////////////////////////////
// 描画関数
/////////////////////////////////////////////////////////////////////////////////////
void CollisionManager::Draw(){
#ifdef _DEBUG
    if(instance_->isDrawCollider_){
        for(auto& collider : instance_->colliderList_){
            collider.second->Draw();
        }
    }
#endif // _DEBUG
}

/////////////////////////////////////////////////////////////////////////////////////
// 当たり判定関数
/////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::CheckCollision(){

    // 渡されたコライダーの更新
    for(auto& collider : instance_->colliderList_){
        collider.second->BeginFrame();
        collider.second->Update();
    }


    // 当たり判定(フィールド vs フィールド上のもの)
    for(int i = 0; i < instance_->onFieldObjectColliders_.size(); i++){
        for(int j = 0; j < instance_->fieldColliders_.size(); j++){
            instance_->onFieldObjectColliders_[i]->CheckCollision(instance_->fieldColliders_[j]);
        }
    }

    // 当たり判定(フィールド上のもの同士)
    for(int i = 0; i < instance_->onFieldObjectColliders_.size(); i++){
        for(int j = i + 1; j < instance_->onFieldObjectColliders_.size(); j++){
            instance_->onFieldObjectColliders_[i]->CheckCollision(instance_->onFieldObjectColliders_[j]);
        }
    }
   
    // エディターのコライダーはすべてと当たり判定を取る
    for(int i = 0; i < instance_->editorColliders_.size(); i++){
        for(int j = 0; j < instance_->onFieldObjectColliders_.size(); j++){
            instance_->editorColliders_[i]->CheckCollision(instance_->onFieldObjectColliders_[j]);
        }
    }

    for(int i = 0; i < instance_->editorColliders_.size(); i++){
        for(int j = 0; j < instance_->fieldColliders_.size(); j++){
            instance_->editorColliders_[i]->CheckCollision(instance_->fieldColliders_[j]);
        }
    }

    for(int i = 0; i < instance_->editorColliders_.size(); i++){
        for(int j = i + 1; j < instance_->editorColliders_.size(); j++){
            instance_->editorColliders_[i]->CheckCollision(instance_->editorColliders_[j]);
        }
    }

    //instance_->octree_->CheckCollision();

}


/////////////////////////////////////////////////////////////////////////////////////
// コライダーの追加・削除関数
/////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::ResetColliderList(){
    instance_->octree_->ResetColiderList();
    instance_->colliderList_.clear();
    instance_->fieldColliders_.clear();
    instance_->onFieldObjectColliders_.clear();
    instance_->editorColliders_.clear();
}

void CollisionManager::ResetOctree(const AABB& range, int32_t depth){
    instance_->octree_.reset(new Octree(range, depth, 0, nullptr));
}

void CollisionManager::AddCollider(Collider* object){
    if(instance_->colliderList_.find(object->GetColliderID()) == instance_->colliderList_.end()){
        //instance_->octree_->AddCollider(object);
        instance_->colliderList_[object->GetColliderID()] = object;

        // フィールドの場合
        if((int)object->GetObjectType() & (int)ObjectType::Field){
            instance_->fieldColliders_.push_back(object);

        } // フィールド上のオブジェクトの場合(フィールドと当たり判定を取る場合)
        else if ((int)object->GetObjectType() & (int)ObjectType::OnFieldObject)
        {
            instance_->onFieldObjectColliders_.push_back(object);
        } else{
            instance_->editorColliders_.push_back(object);
        }
    }
}