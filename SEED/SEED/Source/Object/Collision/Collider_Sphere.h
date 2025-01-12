#pragma once
#include "Collider.h"

class Collider_Sphere : public Collider{
public:
    Collider_Sphere();
    ~Collider_Sphere()override;
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
    Sphere GetSphere()const{ return body_; }
    Sphere GetPreSphere()const{ return preBody_; }

    void SetCenter(const Vector3& center){ local_.center = center; }
    void AddCenter(const Vector3& center){ local_.center += center; }
    void SetRadius(float radius){ body_.radius = radius; }
    bool IsMoved()override;

private:
    Sphere local_;
    Sphere body_;
    Sphere preBody_;
};