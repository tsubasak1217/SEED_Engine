#pragma once
#include <list>
#include "Collision/Collider.h"

class CollisionManager{
private:
    // シングルトン
    CollisionManager() = default;
    CollisionManager* instance_ = nullptr;
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;

public:
    ~CollisionManager();

public:// 当たり判定関数-------------------------------------------------------------------
    void CheckCollision();


public:// コライダーの追加・削除関数---------------------------------------------------------
    void ResetColliderList();
    void AddCollider(Collider* object);
    void AddCollider(std::list<Collider*> objects);

private:// メンバ変数---------------------------------------------------------------------

    // すべてのコライダー
    std::list<Collider*> colliders_;
    // フィールドコライダー
    std::list<Collider*> fieldColliders_;
    // 敵、障害物コライダー
    std::list<Collider*> enemyColliders_;
    // 味方コライダー
    std::list<Collider*> allyColliders_;
};