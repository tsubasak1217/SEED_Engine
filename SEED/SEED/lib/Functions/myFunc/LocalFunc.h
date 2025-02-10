#pragma once

#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>

Matrix4x4 LookAtMatrix(const Vector3& position, const Vector3& target, const Vector3& up);

Vector3 MatrixToEulerAngles(const Matrix4x4& rotationMatrix);

Vector3 TransformVector(const Vector3& vector, const Matrix4x4& matrix);

float LengthSquared(const Vector3& v);

std::string GenerateGUID();

Vector3 GetForward(const Vector3& rotate);