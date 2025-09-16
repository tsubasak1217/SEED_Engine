#include "Pipeline.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 初期化関数
// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Pipeline::Pipeline(BlendMode blendMode, PolygonTopology topology, D3D12_CULL_MODE cullMode){
    Create(blendMode, topology, cullMode);
}

void Pipeline::Create(BlendMode blendMode,PolygonTopology topology,D3D12_CULL_MODE cullMode){

    //======================================================================
    //  Topology
    //======================================================================
    primitiveTopology_ = (topology == PolygonTopology::TRIANGLE) ?
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D_PRIMITIVE_TOPOLOGY_LINELIST;

    D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = (topology == PolygonTopology::TRIANGLE) ?
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE : D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

    //======================================================================
    //  BlendMode
    //======================================================================

    D3D12_BLEND_DESC bDesc{};
    blendMode_ = blendMode;
    // すべての色を書き込むよう設定
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    bDesc.RenderTarget[0].BlendEnable = TRUE;  // ブレンドを有効にする

    // ブレンドモードに応じた設定
    if(blendMode == BlendMode::NORMAL){
        bDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        bDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        bDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        bDesc.AlphaToCoverageEnable = FALSE;
        bDesc.IndependentBlendEnable = FALSE;

    } else if(blendMode == BlendMode::ADD){
        bDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        bDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
        bDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

    } else if(blendMode == BlendMode::SUBTRACT){
        bDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        bDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
        bDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;

    } else if(blendMode == BlendMode::MULTIPLY){
        bDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        bDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
        bDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

    } else if(blendMode == BlendMode::SCREEN){
        bDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
        bDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
        bDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

    } else{
        bDesc.RenderTarget[0].BlendEnable = FALSE;
    }

    // アルファチャンネルのブレンド
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;  // アルファ値はそのまま
    bDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO; 
    bDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;  // アルファの加算

    //======================================================================
    //  Rasterizer
    //======================================================================

    D3D12_RASTERIZER_DESC rasterizerDesc{};
    if(topology == PolygonTopology::TRIANGLE){
        rasterizerDesc.CullMode = cullMode;// 裏面を表示しない
    } else{
        rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;// 表示
    }

    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;// 三角形の中を塗りつぶす
    rasterizerDesc.MultisampleEnable = FALSE; // アンチエイリアシング無効化
    rasterizerDesc.AntialiasedLineEnable = FALSE; // ラインアンチエイリアシング無効化



    //======================================================================
    //  Depth
    //======================================================================

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;// Depth機能有効化

    if(blendMode == BlendMode::ADD or blendMode == BlendMode::SCREEN or blendMode == BlendMode::SUBTRACT){
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;// 書き込みしない
    } else{
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;// 書き込みする
    }

    // 値があればそれを使用
    if(depthWriteMask_.has_value()){
        depthStencilDesc.DepthWriteMask = depthWriteMask_.value();
    }

    depthStencilDesc.DepthFunc = depthFunc_;


    //======================================================================
    //  Sampler
    //======================================================================
    DXGI_SAMPLE_DESC sampleDesc{};
    sampleDesc.Count = 1; // サンプリングカウント。1固定
    sampleDesc.Quality = 0;

    //======================================================================
    //  Format
    //======================================================================

    D3D12_RT_FORMAT_ARRAY rtFormats{};
    rtFormats.NumRenderTargets = 1;
    rtFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 書き込むRTVの情報

    //======================================================================
    // Shader
    //======================================================================

    // VS
    D3D12_SHADER_BYTECODE vsByteCode{};
    vsByteCode.BytecodeLength = pVsBlob_->GetBufferSize();
    vsByteCode.pShaderBytecode = pVsBlob_->GetBufferPointer();

    // PS
    D3D12_SHADER_BYTECODE psByteCode{};
    psByteCode.BytecodeLength = pPsBlob_->GetBufferSize();
    psByteCode.pShaderBytecode = pPsBlob_->GetBufferPointer();

    //======================================================================
    //  PipelineStateDescの情報をまとめる
    //======================================================================
    pipelineDescs_.blend = bDesc;
    pipelineDescs_.rasterizer = rasterizerDesc;
    pipelineDescs_.depthStencil = depthStencilDesc;
    pipelineDescs_.dsFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pipelineDescs_.rtFormats = rtFormats;
    pipelineDescs_.sampleDesc = sampleDesc;
    pipelineDescs_.sampleMask = UINT_MAX;
    pipelineDescs_.flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    pipelineDescs_.vs = vsByteCode;
    pipelineDescs_.ps = psByteCode;
    pipelineDescs_.primitiveTopologyType = topologyType;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 解放関数
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void Pipeline::Release(){
    pipeline_.Reset();
    inputElementDescs_.clear();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 入力レイアウトの追加
// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void Pipeline::AddInputElementDesc(
    LPCSTR semanticName,
    UINT semanticIndex,
    UINT inputSlot, 
    DXGI_FORMAT format, 
    D3D12_INPUT_CLASSIFICATION inputSlotClass, 
    UINT alignedByteOffset
){
    // セマンティクス名の追加
    semanticNames_[inputElementDescs_.size()] = semanticName;

    // 入力レイアウトの設定
    D3D12_INPUT_ELEMENT_DESC elementDesc = {};
    elementDesc.SemanticName = semanticNames_[inputElementDescs_.size()].c_str();
    elementDesc.SemanticIndex = semanticIndex;
    elementDesc.Format = format;
    elementDesc.InputSlot = inputSlot;
    elementDesc.AlignedByteOffset = alignedByteOffset;
    elementDesc.InputSlotClass = inputSlotClass;

    // 配列に追加し情報を更新
    inputElementDescs_.push_back(elementDesc);
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
    inputLayoutDesc.pInputElementDescs = inputElementDescs_.data();
    inputLayoutDesc.NumElements = (UINT)inputElementDescs_.size();
    pipelineDescs_.inputLayoutDesc = inputLayoutDesc;
}
