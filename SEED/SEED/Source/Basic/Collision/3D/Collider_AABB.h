#pragma once
#include "Collider.h"

namespace SEED{
    /// <summary>
    /// 3次元AABBコライダー
    /// </summary>
    class Collider_AABB : public Collider{
    public:
        Collider_AABB();
        ~Collider_AABB()override;
        void UpdateMatrix() override;
        void Draw() override;
        void BeginFrame()override;
        void CheckCollision(Collider* collider)override;
        bool CheckCollision(const Topology::AABB& aabb)override;
        bool CheckCollision(const Topology::OBB& obb)override;
        bool CheckCollision(const Topology::Line& line)override;
        bool CheckCollision(const Topology::Sphere& sphere)override;

    protected:
        void UpdateBox()override;

    public:
        void DiscardPreCollider()override;
        void Edit()override;
        nlohmann::json GetJsonData()override;
        void LoadFromJson(const nlohmann::json& jsonData)override;

    public:
        Topology::AABB GetAABB()const{ return body_; }
        Topology::AABB GetPreAABB()const{ return preBody_; }

        void SetCenter(const Vector3& center){ local_.center = center; }
        void AddCenter(const Vector3& center){ local_.center += center; }
        void SetSize(const Vector3& size){ body_.halfSize = size * 0.5f; }
        bool IsMoved()override;

    private:
        Topology::AABB local_;
        Topology::AABB body_;
        Topology::AABB preBody_;
    };
}