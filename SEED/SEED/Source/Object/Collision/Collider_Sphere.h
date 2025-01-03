#pragma once
#include "Collider.h"

class Collider_Sphere : public Collider{
public:
    Collider_Sphere();
    ~Collider_Sphere() = default;
    void UpdateMatrix() override;
    void Draw() override;
    void CheckCollision(Collider* collider)override;

protected:
    void UpdateBox()override;

public:
    Sphere GetSphere()const{ return body_; }
    void SetCenter(const Vector3& center){ local_.center = center; }
    void SetRadius(float radius){ body_.radius = radius; }

private:
    Sphere local_;
    Sphere body_;
};