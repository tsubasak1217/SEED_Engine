#pragma once
#include <SEED/Source/Object/Particle/BaseParticle.h>

class RadialParticle : public BaseParticle{
public:
    RadialParticle() = default;
    RadialParticle(Emitter_Base* emitter);
    ~RadialParticle() = default;

    void Update()override;

public:
    // パーティクルの位置を取得
    const Vector3& GetPos() override{ return particle_->transform_.translate_; }

private:

};