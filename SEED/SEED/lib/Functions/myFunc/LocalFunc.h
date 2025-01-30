#pragma once

#include "MyFunc.h"
#include "MyMath.h"
#include "MatrixFunc.h"


Matrix4x4 LookAtMatrix(const Vector3& position, const Vector3& target, const Vector3& up);

Vector3 MatrixToEulerAngles(const Matrix4x4& rotationMatrix);

Vector3 TransformVector(const Vector3& vector, const Matrix4x4& matrix);

float LengthSquared(const Vector3& v);

std::string GenerateGUID();