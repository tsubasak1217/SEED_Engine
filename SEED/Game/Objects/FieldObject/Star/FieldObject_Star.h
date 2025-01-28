#pragma once
#include "FieldObject/FieldObject.h"
#include "PointLight.h"

class FieldObject_Star : public FieldObject{
public:
    FieldObject_Star();
    FieldObject_Star(const std::string& modelName);
    ~FieldObject_Star() = default;
public:
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void OnCollision(const BaseObject* other, ObjectType objectType)override;

public:
    std::unique_ptr<PointLight> light_;
    static uint32_t nextFieldObjectID_;
};