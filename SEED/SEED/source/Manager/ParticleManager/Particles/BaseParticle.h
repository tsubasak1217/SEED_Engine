#pragma once
#include "SEED.h"
#include "SEED.h"
#include "Range3D.h"
#include "Range1D.h"
#include "MyFunc.h"
#include <initializer_list>
#include <memory>

class BaseParticle{
public:
    BaseParticle() = default;
    BaseParticle(
        const Range3D& positionRange,
        const Range1D& radiusRange,
        const Range1D& speedRange,
        float lifeTime,
        const std::initializer_list<Vector4>& colors,
        BlendMode blendMode = BlendMode::ADD
    );
    virtual ~BaseParticle() = default;

    virtual void Update();
    virtual void Draw();

public:

    bool GetIsAlive()const{return lifeTime_ > 0.0f;}
    void SetAcceleration(const Vector3& acceleration){ acceleration_ = acceleration; }
    const Vector3& GetPos()const{ return particle_->translate_; }

protected:

    std::unique_ptr<Model>particle_;
    Vector3 direction_;
    float speed_;
    Vector3 velocity_;
    Vector3 acceleration_;
    Vector3 totalAcceleration_;
    float kLifeTime_;
    float lifeTime_;
};