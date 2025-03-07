#pragma once
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Lib/Tensor/Quaternion.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <DirectXTex.h>

#include "d3dx12.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

struct EulerTransform {
    Vector3 scale_;
    Vector3 rotate_;
    Vector3 translate_;
};

struct QuaternionTransform{
    Vector3 scale_;
    Quaternion rotate_;
    Vector3 translate_;
};

struct TransformMatrix {
    Matrix4x4 WVP_;
    Matrix4x4 world_;
    Matrix4x4 worldInverseTranspose_;
};