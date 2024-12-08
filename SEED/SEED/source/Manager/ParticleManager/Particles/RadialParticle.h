#pragma once
#include "BaseParticle.h"


class RadialParticle : public BaseParticle{
public:
    RadialParticle() = default;
    RadialParticle(const Emitter& emitter);
    ~RadialParticle() = default;

    void Update()override;
};