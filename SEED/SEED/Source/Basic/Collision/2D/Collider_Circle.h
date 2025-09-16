#pragma once
#include "Collider2D.h"

class Collider_Circle : public Collider2D{
public:
    Collider_Circle();
    ~Collider_Circle()override;
    void UpdateMatrix() override;
    void Draw() override;
    void BeginFrame()override;
    void CheckCollision(Collider2D* collider)override;
    bool CheckCollision(const Quad2D& aabb)override;
    bool CheckCollision(const Circle& sphere)override;

protected:
    void UpdateBox()override;

public:
    void DiscardPreCollider()override;
    void Edit()override;
    nlohmann::json GetJsonData()override;
    void LoadFromJson(const nlohmann::json& jsonData)override;

public:
    Circle GetCircle()const{ return body_; }
    Circle GetPreCircle()const{ return preBody_; }

    void SetCenter(const Vector2& center){ local_.center = center; }
    void AddCenter(const Vector2& center){ local_.center += center; }
    void SetRadius(float radius){ body_.radius = radius; }
    bool IsMoved()override;

private:
    Circle local_;
    Circle body_;
    Circle preBody_;
};