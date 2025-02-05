#pragma once
#include "FieldObject/FieldObject.h"

class FieldObject_Chikuwa : public FieldObject{
public:
    FieldObject_Chikuwa();
    ~FieldObject_Chikuwa() = default;
    void Update()override;
    void OnCollision(BaseObject* other,ObjectType objectType)override;
private:
    bool isTouched_ = false;
    float dropWaitTime_ = 2.0f;
};
