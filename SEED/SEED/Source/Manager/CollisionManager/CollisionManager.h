#pragma once
// stl
#include <list>
#include <array>
#include <memory>
// Collider
#include <SEED/Source/Basic/Collision/Collider.h>
#include <SEED/Source/Basic/Collision/Collider2D.h>
#include <SEED/Source/Basic/Collision/Collider_Sphere.h>
#include <SEED/Source/Basic/Collision/Collider_AABB.h>
#include <SEED/Source/Basic/Collision/Collider_OBB.h>
#include <SEED/Source/Basic/Collision/Collider_Line.h>
#include <SEED/Source/Basic/Collision/Collider_Capsule.h>
#include <SEED/Source/Basic/Collision/Collider_Plane.h>
#include <SEED/Source/Basic/Collision/Octree.h>

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
    static void AddCollider(Collider2D* object);

private:// メンバ変数---------------------------------------------------------------------

    // 八分木
    std::unique_ptr<Octree> octree_ = nullptr;
    bool isDrawCollider_;

    // コライダー辞書
    std::unordered_map<uint32_t, Collider*> colliderList3D_;
    std::unordered_map<uint32_t, Collider2D*> colliderList2D_;
    // 種類ごとに分ける
    std::vector<Collider*>fieldColliders3D_;// 静的なフィールド
    std::vector<Collider2D*>fieldColliders2D_;
    std::vector<Collider*>onFieldObjectColliders3D_;// フィールド上を動くもの
    std::vector<Collider2D*>onFieldObjectColliders2D_;
    std::vector<Collider*>editorColliders3D_;// エディター用(全部と当たり判定を取る)
    std::vector<Collider2D*>editorColliders2D_;
};