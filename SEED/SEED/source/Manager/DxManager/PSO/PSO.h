#pragma once
#include <DxFunc.h>
using Microsoft::WRL::ComPtr;

// パイプラインステートの情報を格納する構造体
struct PSO{

    ComPtr<ID3D12PipelineState> pipelineState;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
    D3D12_INPUT_LAYOUT_DESC inputLayout;
    D3D12_RASTERIZER_DESC rasterizer;
    D3D12_BLEND_DESC blend;
    D3D12_DEPTH_STENCIL_DESC depthStencil;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType;
    D3D12_PRIMITIVE_TOPOLOGY topology;
    D3D12_CULL_MODE cullMode;
    D3D12_FILL_MODE fillMode;
};