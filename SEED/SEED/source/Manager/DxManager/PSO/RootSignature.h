#pragma once
#include <array>
#include <DxFunc.h>
using Microsoft::WRL::ComPtr;

// ルートシグネチャの情報を格納する構造体
struct RootSignature{
    RootSignature();
    void AddParameter(
        D3D12_ROOT_PARAMETER_TYPE type,
        D3D12_SHADER_VISIBILITY visibility, 
        UINT shaderRegister, 
        UINT registerSpace = 0
    );

    void AddDescriptorTable(
        D3D12_DESCRIPTOR_RANGE_TYPE type,
        UINT numDescriptors,
        UINT baseShaderRegister,
        D3D12_SHADER_VISIBILITY visibility,
        UINT registerSpace = 0
    );

    void Release();

    ComPtr<ID3D12RootSignature> rootSignature;
    D3D12_ROOT_SIGNATURE_DESC desc;
    std::array<D3D12_ROOT_PARAMETER, 16> parameters{};
    std::array<D3D12_DESCRIPTOR_RANGE, 8> ranges{};
    std::array<D3D12_STATIC_SAMPLER_DESC, 4> samplers{};

private:
    int32_t parameterCount = 0;
    int32_t rangeCount = 0;
    int32_t samplerCount = 0;
};