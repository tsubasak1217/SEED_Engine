#pragma once
#include "BaseParticle.h"

enum class

class PrimitiveParticle : public BaseParticle{
public:
    PrimitiveParticle() = default;
    PrimitiveParticle(const Emitter& emitter);
    ~PrimitiveParticle() = default;

    void Update()override;
};