#pragma once
#include <vector>
#include <memory>
#include <SEED/Source/Basic/Collision/3D/Collider.h>
#include <SEED/Source/Manager/CollisionManager/Collision.h>
#include <SEED/Lib/Shapes/AABB.h>

namespace SEED{
    /// <summary>
    /// 当たり判定用の八分木
    /// </summary>
    class Octree{
    public:
        Octree() = default;
        Octree(const Topology::AABB& range, int depth, int maxDepth, const Octree* parent){
            range_ = range;
            depth_ = depth;
            maxDepth_ = maxDepth;
            parent_ = parent;
        }

        void DrawCollider();

        void ResetColiderList();
        void AddCollider(Collider* object);
        void CheckCollision();

    private:
        uint32_t CalculateMortonCode(const Vector3& position)const;
        uint32_t InterleaveBits(uint32_t n)const;

    public:
        const Octree* parent_ = nullptr;// 親
        std::vector<std::unique_ptr<Octree>> children_;// 子
        std::vector<Collider*> colliders_;// コライダー
        Topology::AABB range_;// 範囲
        uint32_t mortonCode_ = 0;// モートンコード
        int depth_ = 0;// 深さ
        int maxDepth_ = 0;// 最大深さ
    };
}