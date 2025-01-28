#pragma once
#include "../FieldObject.h" 
#include "Sprite.h"
#include <memory>

class FieldObject_Goal
    : public FieldObject{
public:
    FieldObject_Goal();
    FieldObject_Goal(const std::string& modelName);
    ~FieldObject_Goal() = default;

public:
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void EndFrame()override;
    void OnCollision(const BaseObject* other, ObjectType objectType)override;
    void ShowImGui()override{}

public:
    static uint32_t nextFieldObjectID_;
    bool isGoal_ = false;
private:
    std::unique_ptr<Sprite> goalUI_;
    bool isTouchedPlayer_ = false;
    float playerTouchTime_ = 0.0f;
};
