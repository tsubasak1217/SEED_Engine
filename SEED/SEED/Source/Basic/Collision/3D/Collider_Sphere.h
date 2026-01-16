#pragma once
#include "Collider.h"

namespace SEED{
    /// <summary>
    /// 球コライダー
    /// </summary>
    class Collider_Sphere : public Collider{
    public:
        Collider_Sphere();
        ~Collider_Sphere()override;
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
        Topology::Sphere GetSphere()const{ return body_; }
        Topology::Sphere GetPreSphere()const{ return preBody_; }

        void SetCenter(const Vector3& center){ local_.center = center; }
        void AddCenter(const Vector3& center){ local_.center += center; }
        void SetRadius(float radius){ body_.radius = radius; }
        bool IsMoved()override;

    private:
        Topology::Sphere local_;
        Topology::Sphere body_;
        Topology::Sphere preBody_;
    };
}