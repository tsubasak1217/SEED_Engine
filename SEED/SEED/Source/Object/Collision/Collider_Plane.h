#pragma once
#include "Collider.h"

class Collider_Plane : public Collider{
public:
    Collider_Plane();
    ~Collider_Plane()override = default;
    void UpdateMatrix() override;
    void Draw() override;
    void CheckCollision(Collider* collider)override;

protected:
    void UpdateBox()override;

public:
    void Edit()override;
    nlohmann::json GetJsonData()override;

public:
    Quad GetPlane()const{ return body_; }
    void SetLocalVertices(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& v3);

private:
    Quad local_;
    Quad body_;
};