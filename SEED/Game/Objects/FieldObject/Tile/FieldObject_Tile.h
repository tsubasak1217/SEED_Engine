#pragma once
#include "FieldObject/FieldObject.h"

class FieldObject_Tile : public FieldObject{
public:
    FieldObject_Tile();
    ~FieldObject_Tile() = default;
public:
    void Initialize()override;
};
