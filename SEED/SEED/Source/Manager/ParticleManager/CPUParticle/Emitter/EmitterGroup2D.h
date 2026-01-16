#pragma once
#include "EmitterGroupBase.h"

namespace SEED{
    /// <summary>
    /// 2Dエミッターをグループで管理するクラス
    /// </summary>
    class EmitterGroup2D : public EmitterGroupBase{
        friend class ParticleManager;
        friend class Component_EmitterGroup2D;
    public:
        EmitterGroup2D();
        EmitterGroup2D(const Matrix3x3* parentMat) : parentMat(parentMat){}
        ~EmitterGroup2D() = default;

    public:
        // アクセッサ
        Vector2 GetPosition() const;

    private:
        const Matrix3x3* parentMat = nullptr;
        Vector2 offset;

    };
}