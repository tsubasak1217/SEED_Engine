#pragma once
#include <list>
#include "Collision/Collider.h"

class CollisionManager{
private:
    // シングルトン
    CollisionManager() = default;
    static CollisionManager* instance_;
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;

public:
    ~CollisionManager();
    static CollisionManager* GetInstance();
    static void Initialize();

public:// 当たり判定関数-------------------------------------------------------------------
    static void CheckCollision();


public:// コライダーの追加・削除関数---------------------------------------------------------
    static void ResetColliderList();
    static void AddCollider(Collider* object);
    static void AddColliders(std::list<Collider*> objects);
    static void AddColliders(std::vector<Collider*> objects);

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