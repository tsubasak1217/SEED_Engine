#pragma once
#include "FieldObject/FieldObject.h"

class FieldObject_GroundCube : public FieldObject{
public:
    FieldObject_GroundCube();
    FieldObject_GroundCube(const std::string& modelName);
    ~FieldObject_GroundCube() = default;
public:
    void Initialize()override;
};
