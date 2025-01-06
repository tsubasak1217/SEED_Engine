#pragma once
#include <list>
#include <array>
#include <memory>
#include "Collision/Collider.h"
#include "Collision/Collider_Sphere.h"
#include "Collision/Collider_AABB.h"
#include "Collision/Collider_OBB.h"
#include "Collision/Collider_Line.h"
#include "Collision/Collider_Capsule.h"
#include "Collision/Collider_Plane.h"
#include "Collision/Octree.h"

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
    static void Draw();

public:// 当たり判定関数-------------------------------------------------------------------
    static void CheckCollision();

public:// コライダーの追加・削除関数---------------------------------------------------------
    static void ResetColliderList();
    static void ResetOctree(const AABB& range,int32_t depth);
    static void AddCollider(Collider* object);

private:// メンバ変数---------------------------------------------------------------------

    // 八分木
    std::unique_ptr<Octree> octree_ = nullptr;
    bool isDrawCollider_ = true;

    // コライダー辞書
    std::unordered_map<uint32_t, Collider*> colliderList_;
};