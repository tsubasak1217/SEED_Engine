#pragma once
#include <array>
#include <SEED/Lib/Functions/DxFunc.h>
#include <string>
#include <unordered_map>
#include <variant>
using Microsoft::WRL::ComPtr;

namespace SEED{
    /// <summary>
    /// 各ルートパラメーターを格納する構造体
    /// </summary>
    struct Parameter{
        int32_t parameterIndex = -1;
        std::variant<D3D12_GPU_DESCRIPTOR_HANDLE, D3D12_GPU_VIRTUAL_ADDRESS, void*> bindInfo;
    };

    /// <summary>
    // ルートシグネチャの情報を格納する構造体(シェーダに渡す変数情報など)
    /// </summary>
    struct RootSignature{
        RootSignature();
        void AddParameter(
            const std::string& name,
            D3D12_ROOT_PARAMETER_TYPE type,
            D3D12_SHADER_VISIBILITY visibility,
            UINT shaderRegister,
            UINT registerSpace = 0,
            UINT num32BitValues = 0
        );

        void AddDescriptorTable(
            const std::string& name,
            D3D12_DESCRIPTOR_RANGE_TYPE type,
            UINT numDescriptors,
            UINT baseShaderRegister,
            D3D12_SHADER_VISIBILITY visibility,
            UINT registerSpace = 0
        );

        void SetBindInfo(
            const std::string& variableName,
            std::variant<D3D12_GPU_DESCRIPTOR_HANDLE, D3D12_GPU_VIRTUAL_ADDRESS, void*> info
        );

        void BindAll(ID3D12GraphicsCommandList* commandList, bool isCSRootSignature = false);

        void Release();

        ComPtr<ID3D12RootSignature> rootSignature;
        D3D12_ROOT_SIGNATURE_DESC desc;
        std::array<D3D12_ROOT_PARAMETER, 32> parameters{};
        std::array<D3D12_DESCRIPTOR_RANGE, 16> ranges{};
        std::array<D3D12_STATIC_SAMPLER_DESC, 4> samplers{};

    private:
        int32_t parameterCount = 0;
        int32_t rangeCount = 0;
        int32_t samplerCount = 0;

        // RootParameterのインデックスを変数名で格納するマップ
        std::unordered_map<std::string, Parameter> parameterMap;
    };
}