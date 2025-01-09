#include "Octree.h"
#include "CollisionManaer/CollisionManager.h"

//////////////////////////////////////////////////////////////////////////////
// 所属するコライダーを描画
//////////////////////////////////////////////////////////////////////////////
void Octree::DrawCollider(){
#ifdef _DEBUG
    for(auto& collider : colliders_){
        collider->Draw();
    }
    for(auto& child : children_){
        child->DrawCollider();
    }
#endif
}

//////////////////////////////////////////////////////////////////////////////
// 
// コライダーのリストをリセット
//
//////////////////////////////////////////////////////////////////////////////
void Octree::ResetColiderList(){
    colliders_.clear();
    for(auto& child : children_){
        child->ResetColiderList();
    }
}

//////////////////////////////////////////////////////////////////////////////
// 
// コライダーを追加
// 
//////////////////////////////////////////////////////////////////////////////
void Octree::AddCollider(Collider* object){
    // オブジェクトがこのノードの範囲内に収まっているか確認
    if(!Collision::AABB::AABB(range_, object->GetBox())) {
        return; // 範囲外のオブジェクトは追加しない
    }

    // 最大深さに達している場合はこのノードに追加
    if(depth_ >= maxDepth_) {
        colliders_.push_back(object);
        return;
    }

    // 子ノードがまだ生成されていない場合は分割
    if(children_.empty()) {
        Vector3 center = range_.center;
        Vector3 halfSize = range_.halfSize * 0.5f;

        children_.resize(8);
        children_[0] = std::make_unique<Octree>(AABB(center - halfSize,halfSize), depth_ + 1, maxDepth_, this);// 左下手前
        children_[1] = std::make_unique<Octree>(AABB(center + Vector3(halfSize.x, -halfSize.y, -halfSize.z), halfSize), depth_ + 1, maxDepth_, this);// 右下手前
        children_[2] = std::make_unique<Octree>(AABB(center + Vector3(-halfSize.x, halfSize.y, -halfSize.z), halfSize), depth_ + 1, maxDepth_, this);// 左上手前
        children_[3] = std::make_unique<Octree>(AABB(center + Vector3(halfSize.x, halfSize.y, -halfSize.z), halfSize), depth_ + 1, maxDepth_, this);// 右上手前
        children_[4] = std::make_unique<Octree>(AABB(center + Vector3(-halfSize.x, -halfSize.y, halfSize.z), halfSize), depth_ + 1, maxDepth_, this);// 左下奥
        children_[5] = std::make_unique<Octree>(AABB(center + Vector3(halfSize.x, -halfSize.y, halfSize.z), halfSize), depth_ + 1, maxDepth_, this);// 右下奥
        children_[6] = std::make_unique<Octree>(AABB(center + Vector3(-halfSize.x, halfSize.y, halfSize.z), halfSize), depth_ + 1, maxDepth_, this);// 左上奥
        children_[7] = std::make_unique<Octree>(AABB(center + halfSize, halfSize), depth_ + 1, maxDepth_, this);// 右上奥

    }

    // 子ノードに追加を試みる
    bool addedToChild = false;
    for(auto& child : children_) {
        if(Collision::AABB::AABB(child->range_, object->GetBox())) {
            child->AddCollider(object);
            addedToChild = true;
        }
    }

    // どの子ノードにも追加されなかった場合、このノードに追加
    if(!addedToChild) {
        colliders_.push_back(object);
    }

    //// モートンコードを計算
    //uint32_t morton = CalculateMortonCode(object->GetBox().center);

    //// 子ノードに追加
    //int childIndex = (morton >> (depth_ * 3)) & 0x7;
    //if(!children_.empty() && children_[childIndex]) {
    //    children_[childIndex]->AddCollider(object);
    //} else {
    //    colliders_.push_back(object);
    //}
}

//////////////////////////////////////////////////////////////////////////////
//
// 当たり判定
//
//////////////////////////////////////////////////////////////////////////////
void Octree::CheckCollision(){
    // このノードのコライダー同士の当たり判定
    for(int i = 0; i < colliders_.size(); ++i) {
        for(int j = i + 1; j < colliders_.size(); ++j) {
            colliders_[i]->CheckCollision(colliders_[j]);
        }
    }

    // 子ノード同士の当たり判定
    for(auto& child : children_) {
        child->CheckCollision();
    }
}


//////////////////////////////////////////////////////////////////////////////
//
// モートンコードの計算
//
//////////////////////////////////////////////////////////////////////////////
uint32_t Octree::CalculateMortonCode(const Vector3& position) const{
    Vector3 min = range_.center - range_.halfSize;
    Vector3 max = range_.center + range_.halfSize;

    Vector3 normalized = (position - min) / (max - min);
    uint32_t x = static_cast<uint32_t>(normalized.x * 1023);
    uint32_t y = static_cast<uint32_t>(normalized.y * 1023);
    uint32_t z = static_cast<uint32_t>(normalized.z * 1023);
    return (InterleaveBits(x) | (InterleaveBits(y) << 1) | (InterleaveBits(z) << 2));
}

uint32_t Octree::InterleaveBits(uint32_t n)const{
    n = (n | (n << 16)) & 0x030000FF;
    n = (n | (n << 8)) & 0x0300F00F;
    n = (n | (n << 4)) & 0x030C30C3;
    n = (n | (n << 2)) & 0x09249249;
    return n;
}