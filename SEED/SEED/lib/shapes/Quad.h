#pragma once

#include "Vector4.h"
#include <stdint.h>
#include <cmath>
#include <numbers>
#include "MyMath.h"
#include "MatrixFunc.h"
#include "Material.h"

struct Quad{
    Vector3 localVertex[4];
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
    uint32_t color;
    Vector4 colorf;
    int32_t lightingType_;
    Matrix4x4 uvTransform_;

    Quad(){};
    Quad(
        const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
        const Vector3& scale = { 1.0f,1.0f,1.0f },
        const Vector3& rotate = { 0.0f, 0.0f, 0.0f },
        const Vector3& translate = { 0.0f,0.0f,0.0f },
        uint32_t color = 0xffffffff
    ){
        localVertex[0] = v1;
        localVertex[1] = v2;
        localVertex[2] = v3;
        localVertex[3] = v4;

        this->scale = scale;
        this->rotate = rotate;
        this->translate = translate;

        this->color = color;
        colorf = MyMath::FloatColor(color);
        lightingType_ = LIGHTINGTYPE_HALF_LAMBERT;
        uvTransform_ = IdentityMat4();
    }
};