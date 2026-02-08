#pragma once
#include "Collider2D.h"

namespace SEED{
    /// <summary>
    /// 円コライダー
    /// </summary>
    class Collider_Circle : public Collider2D{
    public:
        Collider_Circle();
        ~Collider_Circle()override;
        void UpdateMatrix() override;
        void Draw() override;
        void BeginFrame()override;
        void CheckCollision(Collider2D* collider)override;
        bool CheckCollision(const Topology::Quad2D& aabb)override;
        bool CheckCollision(const Topology::Circle& sphere)override;

    protected:
        void UpdateBox()override;

    public:
        void DiscardPreCollider()override;
        void Edit()override;
        nlohmann::json GetJsonData()override;
        void LoadFromJson(const nlohmann::json& jsonData)override;

    public:
        Topology::Circle GetCircle()const{ return body_; }
        Topology::Circle GetPreCircle()const{ return preBody_; }

        void SetCenter(const Vector2& center){ local_.center = center; }
        void AddCenter(const Vector2& center){ local_.center += center; }
        void SetRadius(float radius){ body_.radius = radius; }
        bool IsMoved()override;

    private:
        Topology::Circle local_;
        Topology::Circle body_;
        Topology::Circle preBody_;
    };
}