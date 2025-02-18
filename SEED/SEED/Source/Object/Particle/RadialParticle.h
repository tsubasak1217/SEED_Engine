#pragma once
#include <SEED/Source/Object/Particle/BaseParticle.h>


class RadialParticle : public BaseParticle{
public:
    RadialParticle() = default;
    RadialParticle(const Emitter& emitter);
    ~RadialParticle() = default;

    void Update()override;
};