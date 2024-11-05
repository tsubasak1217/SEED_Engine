#pragma once
#include "Camera.h"
#include "RailInfo.h"
#include "Model.h"
#include <memory>

struct RailCamera : public Camera{

public:
    RailCamera();
    ~RailCamera();

    void Update()override;
    void Draw();

public:

    void SetRailInfo(RailInfo* railInfo){ pRailInfo_ = railInfo; }
    bool GetIsDebugCameraActive()const{ return isDebugCameraActive_; }

private:

    bool isRailCameraActive_ = false;
    bool isDebugCameraActive_ = false;
    bool isMove_ = false;
    RailInfo* pRailInfo_ = nullptr;
    float rail_t_ = 0.0f;
    float addValue_ = 0.001f;
    Vector3 targetPoint_;
    Vector3 offset_ = { 0.0f,1.0f,0.0f };
     
    std::unique_ptr<Model>debugModel_;
};