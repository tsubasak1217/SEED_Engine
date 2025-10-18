#pragma once
#include <vector>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Functions/DxFunc.h>
#include <SEED/Source/Manager/DxManager/PSO/IPipeline.h>
using Microsoft::WRL::ComPtr;

struct PipelineDescs{
    StateParam_RootSignature rootSignature;
    StateParam_InputLayoutDesc inputLayoutDesc;
    StateParam_VS vs;
    StateParam_PS ps;
    StateParam_Blend blend;
    StateParam_Rasterizer rasterizer;
    StateParam_DepthStencil depthStencil;
    StateParam_SampleMask sampleMask;
    StateParam_SampleDesc sampleDesc;
    StateParam_DSFormat dsFormat;
    StateParam_RTFormats rtFormats;
    StateParam_Flags flags;
    StateParam_PrimitiveTopologyType primitiveTopologyType;
};


// パイプラインステートの情報を格納する構造体
struct Pipeline : public IPipeline{

public:
    Pipeline() = default;
    Pipeline(
        BlendMode blendMode, 
        PolygonTopology topology = PolygonTopology::TRIANGLE,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    );

    void Create(
        BlendMode blendMode,
        PolygonTopology topology = PolygonTopology::TRIANGLE,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    )override;

    void Release();

public:

    void AddInputElementDesc(
        LPCSTR semanticName,
        UINT semanticIndex,
        UINT inputSlot,
        DXGI_FORMAT format,
        D3D12_INPUT_CLASSIFICATION inputSlotClass,
        UINT alignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT
    );

public:
    PipelineDescs pipelineDescs_;
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;
    std::array<std::string,32> semanticNames_;
};