#pragma once

#include "../FieldObject.h" 

class FieldObject_Start
    : public FieldObject{
public:
    FieldObject_Start();
    FieldObject_Start(const std::string& modelName);
    ~FieldObject_Start() = default;

public:
    void Initialize()override;
    void Update()override;
};