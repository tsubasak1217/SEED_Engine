#pragma once
#include "EmitterGroupBase.h"

// パーティクルをプリセット化するための構造体
class EmitterGroup3D : public EmitterGroupBase{
    friend class Component_EmitterGroup3D;
public:
    EmitterGroup3D();
    EmitterGroup3D(const Matrix4x4* parentMat) : parentMat(parentMat){}
    ~EmitterGroup3D() = default;

public:
    // アクセッサ
    Vector3 GetPosition() const;

private:
    const Matrix4x4* parentMat = nullptr;
    Vector3 offset;
};