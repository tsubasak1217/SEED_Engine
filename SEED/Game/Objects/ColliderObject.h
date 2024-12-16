#pragma once
#include "Vector2.h"

class ColliderObject{
public:
    ColliderObject() = default;
    virtual ~ColliderObject(){};

    void SetPosition(Vector2 position){ position_ = position; }
    Vector2 GetPosition()const{ return position_; }
    void SetRadius(float radius){ radius_ = radius; }
    float GetRadius()const{ return radius_; }

    void Update(Vector2 position,float radius);

protected:
    Vector2 position_;
    float radius_;

};