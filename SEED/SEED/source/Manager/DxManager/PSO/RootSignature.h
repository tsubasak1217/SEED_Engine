#pragma once
#include <vector>
#include <DxFunc.h>
using Microsoft::WRL::ComPtr;

// ルートシグネチャの情報を格納する構造体
struct RootSignature{
    ComPtr<ID3D12RootSignature> rootSignature;
    D3D12_ROOT_SIGNATURE_DESC desc;
    std::vector<D3D12_ROOT_PARAMETER> parameters;
    std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
};