#include "Pipeline.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 初期化関数
// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////

Pipeline::Pipeline(BlendMode blendMode, PolygonTopology topology, D3D12_CULL_MODE cullMode){
    Initialize(blendMode, topology, cullMode);
}

void Pipeline::Initialize(BlendMode blendMode,PolygonTopology topology,D3D12_CULL_MODE cullMode){

    //======================================================================
    //  Topology
    //======================================================================
    
    // 形状の設定
    if(topology == PolygonTopology::TRIANGLE){
        topologyType_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    } else{
        topologyType_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    }


    //======================================================================
    //  BlendMode
    //======================================================================

    D3D12_BLEND_DESC bDesc{};
    // すべての色を書き込むよう設定
    bDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    bDesc.RenderTarget[0].BlendEnable = TRUE;  // ブレンドを有効にする

    // ブレンドモードに応じた設定
    if(blendMode == BlendMode::NORMAL){
        bDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        bDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        bDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

    } else if(blendMode == BlendMode::ADD){
        bDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        bDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        bDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

    } else if(blendMode == BlendMode::SUBTRACT){
        bDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        bDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
        bDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

    } else if(blendMode == BlendMode::MULTIPLY){
        bDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        bDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        bDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;

    } else if(blendMode == BlendMode::SCREEN){
        bDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
        bDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        bDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

    } else{
        bDesc.RenderTarget[0].BlendEnable = FALSE;
    }

    // アルファチャンネルのブレンド
    bDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;  // アルファ値はそのまま
    bDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;  // アルファの加算
    bDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;  // アルファ値に影響しない

    blendDesc_ = bDesc;

    //======================================================================
    //  Rasterizer
    //======================================================================

    if(topology == PolygonTopology::TRIANGLE){
        rasterizerDesc_.CullMode = cullMode;// 裏面を表示しない
    } else{
        rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;// 表示
    }

    rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;// 三角形の中を塗りつぶす
    rasterizerDesc_.MultisampleEnable = FALSE; // アンチエイリアシング無効化
    rasterizerDesc_.AntialiasedLineEnable = FALSE; // ラインアンチエイリアシング無効化

    //======================================================================
    //  Depth
    //======================================================================

    depthStencilDesc_.DepthEnable = true;// Depth機能有効化

    if(blendMode == BlendMode::ADD or blendMode == BlendMode::SCREEN){
        depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;// 書き込みしない
    } else{
        depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;// 書き込みする
    }

    depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;// 近いものを優先して描画
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
    // 入力レイアウトの設定
    D3D12_INPUT_ELEMENT_DESC elementDesc = {};
    elementDesc.SemanticName = semanticName;
    elementDesc.SemanticIndex = semanticIndex;
    elementDesc.Format = format;
    elementDesc.InputSlot = inputSlot;
    elementDesc.AlignedByteOffset = alignedByteOffset;
    elementDesc.InputSlotClass = inputSlotClass;

    // 配列に追加し情報を更新
    inputElementDescs_.push_back(elementDesc);
    inputLayout_.NumElements = (UINT)inputElementDescs_.size();
    inputLayout_.pInputElementDescs = inputElementDescs_.data();
}
