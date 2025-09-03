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
        for(auto& collider : instance_->colliderList3D_){
            collider.second->Draw();
        }

        for(auto& collider : instance_->colliderList2D_){
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
    for(auto& collider : instance_->colliderList3D_){
        collider.second->BeginFrame();
        collider.second->Update();
    }

    for(auto& collider : instance_->colliderList2D_){
        collider.second->BeginFrame();
        collider.second->Update();
    }


    // 当たり判定(フィールド vs フィールド上のもの)
    for(int i = 0; i < instance_->onFieldObjectColliders3D_.size(); i++){
        for(int j = 0; j < instance_->fieldColliders3D_.size(); j++){
            instance_->onFieldObjectColliders3D_[i]->CheckCollision(instance_->fieldColliders3D_[j]);
        }
    }

    for(int i = 0; i < instance_->onFieldObjectColliders2D_.size(); i++){
        for(int j = 0; j < instance_->fieldColliders2D_.size(); j++){
            instance_->onFieldObjectColliders2D_[i]->CheckCollision(instance_->fieldColliders2D_[j]);
        }
    }

    // 当たり判定(フィールド上のもの同士)
    for(int i = 0; i < instance_->onFieldObjectColliders3D_.size(); i++){
        for(int j = i + 1; j < instance_->onFieldObjectColliders3D_.size(); j++){
            instance_->onFieldObjectColliders3D_[i]->CheckCollision(instance_->onFieldObjectColliders3D_[j]);
        }
    }

    for(int i = 0; i < instance_->onFieldObjectColliders2D_.size(); i++){
        for(int j = i + 1; j < instance_->onFieldObjectColliders2D_.size(); j++){
            instance_->onFieldObjectColliders2D_[i]->CheckCollision(instance_->onFieldObjectColliders2D_[j]);
        }
    }
   
    // エディターのコライダーはすべてと当たり判定を取る
    {
        for(int i = 0; i < instance_->editorColliders3D_.size(); i++){
            for(int j = 0; j < instance_->onFieldObjectColliders3D_.size(); j++){
                instance_->editorColliders3D_[i]->CheckCollision(instance_->onFieldObjectColliders3D_[j]);
            }
        }


        for(int i = 0; i < instance_->editorColliders3D_.size(); i++){
            for(int j = 0; j < instance_->fieldColliders3D_.size(); j++){
                instance_->editorColliders3D_[i]->CheckCollision(instance_->fieldColliders3D_[j]);
            }
        }

        for(int i = 0; i < instance_->editorColliders3D_.size(); i++){
            for(int j = i + 1; j < instance_->editorColliders3D_.size(); j++){
                instance_->editorColliders3D_[i]->CheckCollision(instance_->editorColliders3D_[j]);
            }
        }
    }
    {
        for(int i = 0; i < instance_->editorColliders2D_.size(); i++){
            for(int j = 0; j < instance_->onFieldObjectColliders2D_.size(); j++){
                instance_->editorColliders2D_[i]->CheckCollision(instance_->onFieldObjectColliders2D_[j]);
            }
        }
        for(int i = 0; i < instance_->editorColliders2D_.size(); i++){
            for(int j = 0; j < instance_->fieldColliders2D_.size(); j++){
                instance_->editorColliders2D_[i]->CheckCollision(instance_->fieldColliders2D_[j]);
            }
        }
        for(int i = 0; i < instance_->editorColliders2D_.size(); i++){
            for(int j = i + 1; j < instance_->editorColliders2D_.size(); j++){
                instance_->editorColliders2D_[i]->CheckCollision(instance_->editorColliders2D_[j]);
            }
        }
    }
    //instance_->octree_->CheckCollision();

}


/////////////////////////////////////////////////////////////////////////////////////
// コライダーの追加・削除関数
/////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::ResetColliderList(){
    instance_->octree_->ResetColiderList();
    instance_->colliderList3D_.clear();
    instance_->fieldColliders3D_.clear();
    instance_->onFieldObjectColliders3D_.clear();
    instance_->editorColliders3D_.clear();
}

void CollisionManager::ResetOctree(const AABB& range, int32_t depth){
    instance_->octree_.reset(new Octree(range, depth, 0, nullptr));
}

void CollisionManager::AddCollider(Collider* object){
    if(instance_->colliderList3D_.find(object->GetColliderID()) == instance_->colliderList3D_.end()){
        //instance_->octree_->AddCollider(object);
        instance_->colliderList3D_[object->GetColliderID()] = object;

        // フィールドの場合
        if((int)object->GetObjectType() & (int)ObjectType::Field){
            instance_->fieldColliders3D_.push_back(object);

        } // フィールド上のオブジェクトの場合(フィールドと当たり判定を取る場合)
        else if ((int)object->GetObjectType() & (int)ObjectType::OnFieldObject)
        {
            instance_->onFieldObjectColliders3D_.push_back(object);
        } else{
            instance_->editorColliders3D_.push_back(object);
        }
    }
}

void CollisionManager::AddCollider(Collider2D* object){
    if(instance_->colliderList2D_.find(object->GetColliderID()) == instance_->colliderList2D_.end()){
        instance_->colliderList2D_[object->GetColliderID()] = object;
        // フィールドの場合
        if((int)object->GetObjectType() & (int)ObjectType::Field){
            instance_->fieldColliders2D_.push_back(object);
        } // フィールド上のオブジェクトの場合(フィールドと当たり判定を取る場合)
        else if ((int)object->GetObjectType() & (int)ObjectType::OnFieldObject)
        {
            instance_->onFieldObjectColliders2D_.push_back(object);
        } else{
            instance_->editorColliders2D_.push_back(object);
        }
    }
}