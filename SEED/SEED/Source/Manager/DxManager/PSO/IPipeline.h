#pragma once
#pragma warning(disable: 4324)// 自動アライメントの警告を無視する
#include <vector>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
using Microsoft::WRL::ComPtr;

enum class PolygonTopology : int{
    TRIANGLE,
    LINE
};

struct IPipeline{

    virtual ~IPipeline() = default;
    virtual void Create(
        BlendMode blendMode,
        PolygonTopology topology = PolygonTopology::TRIANGLE,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    ) = 0;

    ComPtr<ID3D12PipelineState> pipeline_;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType_;
    D3D12_COMPARISON_FUNC depthFunc_;
    // シェーダー
    IDxcBlob* pVsBlob_;
    IDxcBlob* pPsBlob_;
    IDxcBlob* pMsBlob_;
    IDxcBlob* pAsBlob_;
    IDxcBlob* pCsBlob_;
};


///////////////////////////////////////////////////////////////////////////////
//
// 省略形を定義
//
///////////////////////////////////////////////////////////////////////////////
template<typename ValueType, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE objectType>
class alignas(void*) StateParam{
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

public:
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_;
    ValueType value_;
};

// パイプラインステートのサブオブジェクトの省略形を定義
#define PSST(x) D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_##x
using StateParam_RootSignature = StateParam<ID3D12RootSignature*, PSST(ROOT_SIGNATURE)>;
using StateParam_VS = StateParam<D3D12_SHADER_BYTECODE, PSST(VS)>;
using StateParam_AS = StateParam<D3D12_SHADER_BYTECODE, PSST(AS)>;
using StateParam_MS = StateParam<D3D12_SHADER_BYTECODE, PSST(MS)>;
using StateParam_PS = StateParam<D3D12_SHADER_BYTECODE, PSST(PS)>;
using StateParam_Blend = StateParam<D3D12_BLEND_DESC, PSST(BLEND)>;
using StateParam_Rasterizer = StateParam<D3D12_RASTERIZER_DESC, PSST(RASTERIZER)>;
using StateParam_DepthStencil = StateParam<D3D12_DEPTH_STENCIL_DESC, PSST(DEPTH_STENCIL)>;
using StateParam_SampleMask = StateParam<UINT, PSST(SAMPLE_MASK)>;
using StateParam_SampleDesc = StateParam<DXGI_SAMPLE_DESC, PSST(SAMPLE_DESC)>;
using StateParam_DSFormat = StateParam<DXGI_FORMAT, PSST(DEPTH_STENCIL_FORMAT)>;
using StateParam_RTFormats = StateParam<D3D12_RT_FORMAT_ARRAY, PSST(RENDER_TARGET_FORMATS)>;
using StateParam_Flags = StateParam<D3D12_PIPELINE_STATE_FLAGS, PSST(FLAGS)>;
using StateParam_InputLayoutDesc = StateParam<D3D12_INPUT_LAYOUT_DESC, PSST(INPUT_LAYOUT)>;
#undef PSST
