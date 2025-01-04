#pragma once
#include "Collider.h"

class Collider_Line : public Collider{
public:
    Collider_Line();
    ~Collider_Line() = default;
    void UpdateMatrix() override;
    void Draw() override;

protected:
    void UpdateBox()override;
    
public:
    void Edit(const std::string& headerName)override;

public:
    Line GetLine()const{ return body_; }
    void SetLocalPosition(const Vector3& origin, const Vector3& end){ local_.origin_ = origin; local_.end_ = end; }

private:
    Line local_;
    Line body_;
};