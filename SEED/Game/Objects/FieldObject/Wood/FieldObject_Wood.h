#pragma once
#include "FieldObject/FieldObject.h"

class FieldObject_Wood : public FieldObject{
public:
    FieldObject_Wood();
    ~FieldObject_Wood() = default;
    void Initialize()override;
    void Update()override;
    void Draw()override;

public:
    Vector4 leafColor_ = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
};
