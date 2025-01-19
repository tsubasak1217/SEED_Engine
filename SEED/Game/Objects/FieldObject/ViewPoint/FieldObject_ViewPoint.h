#pragma once

#include "../FieldObject.h" 

class FieldObject_ViewPoint : public FieldObject{
public:
    FieldObject_ViewPoint();
    ~FieldObject_ViewPoint() = default;

public:
    void Initialize()override;
    void Update()override;
    void Draw()override;

public:
    static uint32_t nextFieldObjectID_;
};