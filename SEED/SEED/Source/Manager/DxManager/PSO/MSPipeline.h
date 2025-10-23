#pragma once
#include <vector>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Functions/DxFunc.h>
#include <SEED/Source/Manager/DxManager/PSO/IPipeline.h>
using Microsoft::WRL::ComPtr;

/// <summary>
/// MeshShader用のパイプラインのDesc情報を格納する構造体
/// </summary>
struct MSPipelineDescs{
    StateParam_RootSignature rootSignature;
    StateParam_AS as;
    StateParam_MS ms;
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



/// <summary>
// MeshShaderのパイプラインステートの情報を格納する構造体
/// </summary>
struct MSPipeline : public IPipeline{

public:
    MSPipeline() = default;
    MSPipeline(
        BlendMode blendMode, 
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    );

    void Create(
        BlendMode blendMode,
        PolygonTopology topology = PolygonTopology::TRIANGLE,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    )override;

    void Release();

public:
    // desc情報
    MSPipelineDescs pipelineDescs_;
};