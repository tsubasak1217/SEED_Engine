#pragma once
#include "Collider.h"

namespace SEED{
    /// <summary>
    /// 3Dカプセルコライダー
    /// </summary>
    class Collider_Capsule : public Collider{
    public:
        Collider_Capsule();
        ~Collider_Capsule()override;
        void UpdateMatrix() override;
        void Draw() override;
        void BeginFrame()override;

    protected:
        void UpdateBox()override;

    public:
        void DiscardPreCollider()override;
        void Edit()override;
        nlohmann::json GetJsonData()override;
        void LoadFromJson(const nlohmann::json& jsonData)override;

    public:
        Topology::Capsule GetCapsule()const{ return body_; }
        Topology::Capsule GetPreCapsule()const{ return preBody_; }
        void SetLocalPosition(const Vector3& origin, const Vector3& end){ local_.origin = origin; local_.end = end; }
        void SetRadius(float radius){ body_.radius = radius; }

    private:
        Topology::Capsule local_;
        Topology::Capsule body_;
        Topology::Capsule preBody_;
    };
}