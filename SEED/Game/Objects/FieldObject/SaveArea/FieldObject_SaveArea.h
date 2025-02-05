#pragma once
#include "FieldObject/FieldObject.h"

class FieldObject_SaveArea : public FieldObject{
public:
    FieldObject_SaveArea();
    ~FieldObject_SaveArea() = default;

public:
    void Initialize()override;
    void Draw()override;
    void BeginFrame()override;
    void OnCollision( BaseObject* other, ObjectType objectType)override;

private:
    bool isCollidePlayer_ = false;
    bool preIsCollidePlayer_ = false;
    const static std::string jsonPath_;
};