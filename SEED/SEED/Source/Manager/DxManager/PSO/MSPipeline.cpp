#include "MSPipeline.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// コンストラクタ
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

MSPipeline::MSPipeline(BlendMode blendMode, D3D12_CULL_MODE cullMode){
    Create(blendMode,PolygonTopology::TRIANGLE, cullMode);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 作成関数
// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void MSPipeline::Create(BlendMode blendMode, PolygonTopology topology,D3D12_CULL_MODE cullMode){

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
    rasterizerDesc.CullMode = cullMode;
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
    depthStencilDesc.DepthFunc = depthFunc_;// 近いものを優先して描画


    //======================================================================
    //  Sampler
    //======================================================================
    DXGI_SAMPLE_DESC sampleDesc{};
    sampleDesc.Count = 1; // サンプリングカウント。1固定
    sampleDesc.Quality = 0; // サンプリング品

    //======================================================================
    //  Format
    //======================================================================

    D3D12_RT_FORMAT_ARRAY rtFormats{};
    rtFormats.NumRenderTargets = 1;
    rtFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 書き込むRTVの情報

    //======================================================================
    // Shader
    //======================================================================

    // MS
    D3D12_SHADER_BYTECODE msByteCode{};
    msByteCode.BytecodeLength = pMsBlob_->GetBufferSize();
    msByteCode.pShaderBytecode = pMsBlob_->GetBufferPointer();

    // PS
    D3D12_SHADER_BYTECODE psByteCode{};
    psByteCode.BytecodeLength = pPsBlob_->GetBufferSize();
    psByteCode.pShaderBytecode = pPsBlob_->GetBufferPointer();

    // AS
    D3D12_SHADER_BYTECODE asByteCode{};
    asByteCode.BytecodeLength = 0;
    asByteCode.pShaderBytecode = nullptr;

    //======================================================================
    //  PipelineStateのDesc情報をまとめる
    //======================================================================
    pipelineDescs_.blend = bDesc;
    pipelineDescs_.rasterizer = rasterizerDesc;
    pipelineDescs_.depthStencil = depthStencilDesc;
    pipelineDescs_.dsFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pipelineDescs_.rtFormats = rtFormats;
    pipelineDescs_.sampleDesc = sampleDesc;
    pipelineDescs_.sampleMask = UINT_MAX;
    pipelineDescs_.flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    pipelineDescs_.as = asByteCode;
    pipelineDescs_.ms = msByteCode;
    pipelineDescs_.ps = psByteCode;
    pipelineDescs_.primitiveTopologyType = topologyType;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 解放関数
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void MSPipeline::Release(){
    pipeline_.Reset();
}