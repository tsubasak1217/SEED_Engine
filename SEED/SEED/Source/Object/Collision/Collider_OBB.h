#pragma once
#include "Collider.h"

class Collider_OBB : public Collider{
public:
    Collider_OBB();
    ~Collider_OBB()override = default;
    void UpdateMatrix() override;
    void Draw() override;
    void CheckCollision(Collider* collider)override;

protected:
    void UpdateBox()override;

public:
    void Edit()override;
    nlohmann::json GetJsonData()override;
    void LoadFromJson(const nlohmann::json& jsonData)override;

public:
    OBB GetOBB()const{ return body_; }
    OBB GetPreOBB()const{ 
        Collider_OBB* preObbCollider = dynamic_cast<Collider_OBB*>(preCollider_);
        return preObbCollider->GetOBB();
    }

    void SetCenter(const Vector3& center){ local_.center = center; }
    void SetSize(const Vector3& size){ body_.halfSize = size * 0.5f; }
    bool IsMoved()override;

private:
    OBB local_;
    OBB body_;
};