#pragma once
#include "FieldObject/FieldObject.h"

class FieldObject_GrassSoil : public FieldObject{
public:
    FieldObject_GrassSoil();
    FieldObject_GrassSoil(const std::string& modelName);
    ~FieldObject_GrassSoil() = default;
public:

    void Initialize()override;
};
