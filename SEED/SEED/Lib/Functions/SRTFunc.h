#pragma once
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Tensor/Quaternion.h>
#include <SEED/Lib/Tensor/Matrix2x2.h>
#include <SEED/Lib/Tensor/Matrix3x3.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Lib/Structs/Transform.h>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
using namespace DirectX;

namespace SEED{
    namespace Methods{
        namespace SRT{
            // Quaternionの補間
            Quaternion ComputeTangent(const Quaternion& qprev, const Quaternion& q, const Quaternion& qnext);
            Quaternion Squad(const Quaternion& q1, const Quaternion& q2, const Quaternion& a, const Quaternion& b, float t);
            Quaternion MultiSquad(const std::vector<Quaternion>& quats, float t, bool isConnectEdge = false);

            // トランスフォームのL補間関数
            Transform Interpolate(const Transform& a, const Transform& b, float t);
            Transform2D Interpolate(const Transform2D& a, const Transform2D& b, float t);
            Transform2D Interpolate(const std::vector<Transform2D>& transforms, float t);
            Transform Interpolate(const std::vector<Transform>& transforms, float t);
            Quaternion Interpolate(const std::vector<Quaternion>& transforms, float t);
            Transform2D CatmullRomInterpolate(const std::vector<Transform2D>& transforms, float t, bool connectEdge = false);
            Transform CatmullRomInterpolate(const std::vector<Transform>& transforms, float t, bool connectEdge = false);

            // thetaとphiからベクトルを生成する関数
            Vector3 CreateVector(float theta, float phi);

            // ベクトルから三次元の回転角を算出する関数
            Vector3 CalcRotateFromVec(const Vector3& vec);
        }
    }
}