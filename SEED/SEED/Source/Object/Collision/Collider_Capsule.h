#pragma once
#include "Collider.h"

class Collider_Capsule : public Collider{
public:
    Collider_Capsule();
    ~Collider_Capsule()override = default;
    void UpdateMatrix() override;
    void Draw() override;

protected:
    void UpdateBox()override;

public:
    void Edit()override;
    nlohmann::json GetJsonData()override;
    void LoadFromJson(const nlohmann::json& jsonData)override;

public:
    Capsule GetCapsule()const{ return body_; }
    void SetLocalPosition(const Vector3& origin, const Vector3& end){ local_.origin = origin; local_.end = end; }
    void SetRadius(float radius){ body_.radius = radius; }

private:
    Capsule local_;
    Capsule body_;
};