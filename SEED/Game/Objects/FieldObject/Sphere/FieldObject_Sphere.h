#pragma once
#include "FieldObject/FieldObject.h"

class FieldObject_Sphere : public FieldObject{
public:
    FieldObject_Sphere();
    FieldObject_Sphere(const std::string& modelName);
    ~FieldObject_Sphere() = default;
public:
    void Initialize()override;
};