#pragma once
#include "../FieldObject.h" 
#include "PointLight.h"
#include <memory>

class FieldObject_PointLight : public FieldObject{
public:
    FieldObject_PointLight();
    ~FieldObject_PointLight() = default;

public:
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void Edit()override;
    nlohmann::json OutputJson()override;
    void LoadFromJson(const nlohmann::json& json)override;

public:
    std::unique_ptr<PointLight> pointLight_;
};