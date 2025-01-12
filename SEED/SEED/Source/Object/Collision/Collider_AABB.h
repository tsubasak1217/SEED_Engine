#pragma once
#include "Collider.h"

class Collider_AABB : public Collider{
public:
    Collider_AABB();
    ~Collider_AABB()override;
    void UpdateMatrix() override;
    void Draw() override;
    void BeginFrame()override;
    void CheckCollision(Collider* collider)override;

protected:
    void UpdateBox()override;

public:
    void Edit()override;
    nlohmann::json GetJsonData()override;
    void LoadFromJson(const nlohmann::json& jsonData)override;

public:
    AABB GetAABB()const{ return body_; }
    AABB GetPreAABB()const{ return preBody_; }

    void SetCenter(const Vector3& center){ local_.center = center; }
    void AddCenter(const Vector3& center){ local_.center += center; }
    void SetSize(const Vector3& size){ body_.halfSize = size * 0.5f; }
    bool IsMoved()override;

private:
    AABB local_;
    AABB body_;
    AABB preBody_;
};