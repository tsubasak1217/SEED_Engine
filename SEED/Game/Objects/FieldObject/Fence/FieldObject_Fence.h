#pragma once
#include "FieldObject/FieldObject.h"

class FieldObject_Fence : public FieldObject{
public:
    FieldObject_Fence();
    ~FieldObject_Fence() = default;
public:
    void Initialize()override;
};