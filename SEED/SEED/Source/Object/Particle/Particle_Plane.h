#pragma once
#include <SEED/Source/Object/Particle/BaseParticle.h>

class Particle_Plane : public BaseParticle{
public:
    Particle_Plane() = default;
    Particle_Plane(Emitter_Base* emitter);
    ~Particle_Plane() = default;

    void Update()override;

public:
    // パーティクルの位置を取得
    const Vector3& GetPos() override{ return particle_->transform_.translate_; }

private:

};