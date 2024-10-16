#pragma once
#include "Camera.h"
#include "RailInfo.h"

struct RailCamera : public Camera{

public:
    RailCamera() = default;
    ~RailCamera();

    void Update()override;

public:

    void SetRailInfo(RailInfo* railInfo){ pRailInfo_ = railInfo; }

private:

    bool isRailCameraActive_ = false;
    bool isDebugCameraActive_ = false;
    bool isMove_ = false;
    RailInfo* pRailInfo_ = nullptr;
    float rail_t_;
    float addValue_ = 0.0005f;
    Vector3 targetPoint_;
};