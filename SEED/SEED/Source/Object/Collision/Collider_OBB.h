#pragma once
#include "Collider.h"

class Collider_OBB : public Collider{
public:
    Collider_OBB();
    ~Collider_OBB()override;
    void UpdateMatrix() override;
    void Draw() override;
    void BeginFrame()override;
    void CheckCollision(Collider* collider)override;
    bool CheckCollision(const AABB& aabb)override;
    bool CheckCollision(const OBB& obb)override;
    bool CheckCollision(const Line& line)override;
    bool CheckCollision(const Sphere& sphere)override;

protected:
    void UpdateBox()override;

public:
    void Edit()override;
    nlohmann::json GetJsonData()override;
    void LoadFromJson(const nlohmann::json& jsonData)override;

public:
    OBB GetOBB()const{ return body_; }
    OBB GetPreOBB()const{ return preBody_; }

    void SetCenter(const Vector3& center){ local_.center = center; }
    void AddCenter(const Vector3& center){ local_.center += center; }
    void SetSize(const Vector3& size){ body_.halfSize = size * 0.5f; }
    bool IsMoved()override;

private:
    OBB local_;
    OBB body_;
    OBB preBody_;
};