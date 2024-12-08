#pragma once
#include "BaseParticle.h"


class RadialParticle : public BaseParticle{
public:
    RadialParticle(
        const Range3D& positionRange,
        const Range1D& radiusRange,
        const Vector3& baseDirection,
        float directionRange,
        const Range1D& speedRange,
        const Range1D& lifeTime,
        const std::vector<Vector4>& colors,
        BlendMode blendMode = BlendMode::ADD
    );
    ~RadialParticle() = default;

    void Update()override;
};