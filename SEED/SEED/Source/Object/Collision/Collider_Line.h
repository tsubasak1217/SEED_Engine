#pragma once
#include "Collider.h"

class Collider_Line : public Collider{
public:
    Collider_Line();
    ~Collider_Line()override;
    void UpdateMatrix() override;
    void Draw() override;
    void BeginFrame()override;
    bool CheckCollision(const AABB& aabb)override;
    bool CheckCollision(const OBB& obb)override;
    bool CheckCollision(const Sphere& sphere)override;

protected:
    void UpdateBox()override;

public:
    void Edit()override;
    nlohmann::json GetJsonData()override;
    void LoadFromJson(const nlohmann::json& jsonData)override;

public:
    Line GetLine()const{ return body_; }
    Line GetPreLine()const{ return preBody_; }
    void SetLocalPosition(const Vector3& origin, const Vector3& end){ local_.origin_ = origin; local_.end_ = end; }

private:
    Line local_;
    Line body_;
    Line preBody_;
};