#pragma once
#include <SEED/Source/Object/Particle/BaseParticle.h>

class Particle_Model : public BaseParticle{
public:
    Particle_Model() = default;
    Particle_Model(Emitter_Base* emitter);
    ~Particle_Model() = default;

    void Update()override;

public:
    // パーティクルの位置を取得
    const Vector3& GetPos() override{ return particle_->transform_.translate_; }

private:

};