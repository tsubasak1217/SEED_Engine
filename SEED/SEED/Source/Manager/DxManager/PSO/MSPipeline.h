#pragma once
#include <vector>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
using Microsoft::WRL::ComPtr;


template<typename ValueType, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE objectType>
class StateParam{
public:
    StateParam() = default;
    StateParam(ValueType value) : type_(objectType), value_(value){}
    // コピーコンストラクタ
    StateParam(const StateParam& other) : type_(other.type_), value_(other.value_){}
    // コピー代入演算子
    StateParam& operator=(const StateParam& other){
        if(this != &other){
            type_ = other.type_;
            value_ = other.value_;
        }
        return *this;
    }


private:
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_;
    ValueType value_;
};

// パイプラインステートのサブオブジェクトの省略形を定義
#define PSST(x) D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_##x
using StateParam_RootSignature = StateParam<ID3D12RootSignature*, PSST(ROOT_SIGNATURE)>;
using StateParam_AS = StateParam<ID3D12StateObject*, PSST(AS)>;
using StateParam_MS = StateParam<ID3D12StateObject*, PSST(MS)>;
using StateParam_PS = StateParam<ID3D12StateObject*, PSST(PS)>;
using StateParam_Blend = StateParam<D3D12_BLEND_DESC, PSST(BLEND)>;
using StateParam_Rasterizer = StateParam<D3D12_RASTERIZER_DESC, PSST(RASTERIZER)>;
using StateParam_DepthStencil = StateParam<D3D12_DEPTH_STENCIL_DESC, PSST(DEPTH_STENCIL)>;
using StateParam_SampleMask = StateParam<UINT, PSST(SAMPLE_MASK)>;
using StateParam_SampleDesc = StateParam<DXGI_SAMPLE_DESC, PSST(SAMPLE_DESC)>;
using StateParam_DSFormat = StateParam<DXGI_FORMAT, PSST(DEPTH_STENCIL_FORMAT)>;
using StateParam_RTFormats = StateParam<D3D12_RT_FORMAT_ARRAY, PSST(RENDER_TARGET_FORMATS)>;
using StateParam_Flags = StateParam<D3D12_PIPELINE_STATE_FLAGS, PSST(FLAGS)>;
#undef PSST


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
};

// パイプラインステートの情報を格納する構造体
struct MSPipeline{

public:
    MSPipeline() = default;
    MSPipeline(
        BlendMode blendMode, 
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    );

    void Initialize(
        BlendMode blendMode,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    );

    void Release();

public:
    // 本体
    ComPtr<ID3D12PipelineState> pipeline_;
    // シェーダー
    IDxcBlob* pMsBlob_;
    IDxcBlob* pPsBlob_;
    IDxcBlob* pAsBlob_;
    // desc情報
    MSPipelineDescs pipelineDescs_;
};