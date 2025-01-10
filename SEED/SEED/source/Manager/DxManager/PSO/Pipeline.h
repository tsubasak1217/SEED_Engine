#pragma once
#include "BlendMode.h"
#include <vector>
#include <DxFunc.h>
using Microsoft::WRL::ComPtr;

enum class PolygonTopology : int{
    TRIANGLE,
    LINE
};

// パイプラインステートの情報を格納する構造体
struct Pipeline{

public:
    Pipeline() = default;
    Pipeline(
        BlendMode blendMode, 
        PolygonTopology topology = PolygonTopology::TRIANGLE,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    );

    void Initialize(
        BlendMode blendMode,
        PolygonTopology topology = PolygonTopology::TRIANGLE,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    );

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
    ComPtr<ID3D12PipelineState> pipelineState_;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc_{};
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;
    D3D12_INPUT_LAYOUT_DESC inputLayout_{};
    D3D12_RASTERIZER_DESC rasterizerDesc_{};
    D3D12_BLEND_DESC blendDesc_{};
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
    D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType_;
    D3D12_CULL_MODE cullMode_;
    D3D12_FILL_MODE fillMode_;
    // シェーダー
    IDxcBlob* pVsBlob_;
    IDxcBlob* pPsVlob_;
};