#pragma once
#include "SEED.h"
#include "Bullet.h"
#include "RailCamera.h"
#include "Reticle.h"
#include <memory>
#include <list>

class Player{
public:

    Player();
    Player(const Vector3& position);
    ~Player();

    void Update();
    void Draw();
    void Shoot();

public:
    void SetRailCameraPtr(RailCamera* railCamera){ pRailCamera_ = railCamera; }
    void AddPoint(int point){ score_ += point; }
public:

    RailCamera* pRailCamera_ = nullptr;
    Vector3 forward_ = Vector3(0.0f, 0.0f, 1.0f);
    std::unique_ptr<Model> player_ = nullptr;
    std::unique_ptr<Reticle> reticle_ = nullptr;
    std::list<std::unique_ptr<Bullet>>bullets_;

    bool isBeam_ = false;
    Vector3 beamVec_;

    // ゲージ、スコア
    float shotGage_ = 1.0f;
    int score_ = 0;
};