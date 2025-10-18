#pragma once
#include "EmitterGroupBase.h"

// パーティクルをプリセット化するための構造体
class EmitterGroup2D : public EmitterGroupBase{
    friend class ParticleManager;
public:
    EmitterGroup2D();
    EmitterGroup2D(const Matrix3x3* parentMat) : parentMat(parentMat){}
    ~EmitterGroup2D() = default;

public:
    // アクセッサ
    Vector2 GetCenter() const;

private:
    const Matrix3x3* parentMat = nullptr;
    Vector2 offset;

};