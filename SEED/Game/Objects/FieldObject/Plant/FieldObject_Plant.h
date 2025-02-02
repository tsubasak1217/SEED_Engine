#pragma once
#include "FieldObject/FieldObject.h"

class FieldObject_Plant : public FieldObject{
public:
    FieldObject_Plant();
    ~FieldObject_Plant() = default;
    void Initialize()override;
    void Update()override;
    void Draw()override;

public:
    bool isBloomFlower_ = false;
    Vector4 flowerColor_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    int flowerVolume_ = 10;

private:
    int preFlowerVolume_ = 0;
    std::vector<std::unique_ptr<Model>> flowerModel_;
};
