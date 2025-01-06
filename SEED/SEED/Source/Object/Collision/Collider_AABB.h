#pragma once
#include "Collider.h"

class Collider_AABB : public Collider{
public:
    Collider_AABB();
    ~Collider_AABB()override = default;
    void UpdateMatrix() override;
    void Draw() override;
    void CheckCollision(Collider* collider)override;

protected:
    void UpdateBox()override;

public:
    void Edit()override;
    nlohmann::json GetJsonData()override;

public:
    AABB GetAABB()const{ return body_; }
    void SetCenter(const Vector3& center){ local_.center = center; }
    void SetAABBSize(const Vector3& size){ body_.halfSize = size * 0.5f; }

private:
    AABB local_;
    AABB body_;
};