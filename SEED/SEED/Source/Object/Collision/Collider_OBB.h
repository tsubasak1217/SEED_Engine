#pragma once
#include "Collider.h"

class Collider_OBB : public Collider{
public:
    Collider_OBB();
    ~Collider_OBB() = default;
    void UpdateMatrix() override;
    void Draw() override;
    void CheckCollision(Collider* collider)override;

protected:
    void UpdateBox()override;

public:
    OBB GetOBB()const{ return body_; }
    void SetCenter(const Vector3& center){ local_.center = center; }
    void SetSize(const Vector3& size){ body_.halfSize = size * 0.5f; }

private:
    OBB local_;
    OBB body_;
};