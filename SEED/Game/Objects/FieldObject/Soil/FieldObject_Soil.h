#pragma once
#include "FieldObject/FieldObject.h"

class FieldObject_Soil : public FieldObject{
public:
    FieldObject_Soil();
    FieldObject_Soil(const std::string& modelName);
    ~FieldObject_Soil() = default;
public:
    void Initialize()override;
public:
    static uint32_t nextFieldObjectID_;
};
