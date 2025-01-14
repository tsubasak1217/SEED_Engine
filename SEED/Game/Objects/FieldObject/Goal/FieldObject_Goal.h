#pragma once
#include "../FieldObject.h" 

class FieldObject_Goal
    : public FieldObject{
public:
    FieldObject_Goal();
    FieldObject_Goal(const std::string& modelName);
    ~FieldObject_Goal() = default;

public:
    void Initialize()override;
    void Update()override;
};
