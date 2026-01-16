#pragma once
#include "Collider.h"

namespace SEED{
    /// <summary>
    /// 3D線コライダー
    /// </summary>
    class Collider_Line : public Collider{
    public:
        Collider_Line();
        ~Collider_Line()override;
        void UpdateMatrix() override;
        void Draw() override;
        void BeginFrame()override;
        bool CheckCollision(const Topology::AABB& aabb)override;
        bool CheckCollision(const Topology::OBB& obb)override;
        bool CheckCollision(const Topology::Sphere& sphere)override;

    protected:
        void UpdateBox()override;

    public:
        void DiscardPreCollider()override;
        void Edit()override;
        nlohmann::json GetJsonData()override;
        void LoadFromJson(const nlohmann::json& jsonData)override;

    public:
        Topology::Line GetLine()const{ return body_; }
        Topology::Line GetPreLine()const{ return preBody_; }
        void SetLocalPosition(const Vector3& origin, const Vector3& end){ local_.origin_ = origin; local_.end_ = end; }

    private:
        Topology::Line local_;
        Topology::Line body_;
        Topology::Line preBody_;
    };
}