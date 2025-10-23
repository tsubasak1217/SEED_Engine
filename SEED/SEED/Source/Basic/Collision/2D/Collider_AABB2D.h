#pragma once
#include "Collider2D.h"


/// <summary>
/// 2次元AABBコライダー
/// </summary>
class Collider_AABB2D : public Collider2D{
public:
    Collider_AABB2D();
    ~Collider_AABB2D()override;
    void UpdateMatrix() override;
    void Draw() override;
    void BeginFrame()override;
    void CheckCollision(Collider2D* collider)override;
    bool CheckCollision(const AABB2D& aabb);

protected:
    void UpdateBox()override;

public:
    void DiscardPreCollider()override;
    void Edit()override;
    nlohmann::json GetJsonData()override;
    void LoadFromJson(const nlohmann::json& jsonData)override;

public:
    AABB2D GetAABB()const{ return body_; }
    AABB2D GetPreAABB()const{ return preBody_; }
    Vector2 GetAnchor() const { return anchor_; }

    void SetCenter(const Vector2& center){ local_.center = center; }
    void AddCenter(const Vector2& center){ local_.center += center; }
    void SetSize(const Vector2& size){ local_.halfSize = size * 0.5f; }
    void SetAnchor(const Vector2& anchor) { anchor_ = anchor; }
    bool IsMoved()override;

private:

    AABB2D local_;
    AABB2D body_;
    AABB2D preBody_;

    // AABBアンカー
    Vector2 anchor_;
};