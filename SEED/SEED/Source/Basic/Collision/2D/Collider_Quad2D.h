#pragma once
#include "Collider2D.h"

class Collider_Quad2D : public Collider2D{
public:
    Collider_Quad2D();
    ~Collider_Quad2D()override;
    void UpdateMatrix() override;
    void Draw() override;
    void BeginFrame()override;
    void CheckCollision(Collider2D* collider)override;
    bool CheckCollision(const Circle& aabb)override;
    bool CheckCollision(const Quad2D& obb)override;

protected:
    void UpdateBox()override;

public:
    void DiscardPreCollider()override;
    void Edit()override;
    nlohmann::json GetJsonData()override;
    void LoadFromJson(const nlohmann::json& jsonData)override;

public:
    Quad2D GetQuad2D()const{ return body_; }
    Quad2D GetPreQuad2D()const{ return preBody_; }

    void SetCenter(const Vector2& center){ local_.translate = center; }
    void AddCenter(const Vector2& center){ local_.translate += center; }
    bool IsMoved()override;

private:
    Quad2D local_;
    Quad2D body_;
    Quad2D preBody_;
};