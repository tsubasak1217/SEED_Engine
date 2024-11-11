#pragma once
#include "BaseParticle.h"


class RadialParticle : public BaseParticle{
public:
    RadialParticle(
        const Range3D& positionRange,
        const Range1D& radiusRange,
        const Range1D& speedRange,
        float lifeTime,
        const std::initializer_list<Vector4>& colors,
        BlendMode blendMode = BlendMode::ADD
    );
    ~RadialParticle() = default;

    void Update()override;
};