// local
#include "PSO/PSOManager.h"
#include <DxManager.h>
#include "blendMode.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//                         static変数初期化
/////////////////////////////////////////////////////////////////////////////////////////////

PSOManager* PSOManager::instance_ = nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////
//                          初期化・終了関数
/////////////////////////////////////////////////////////////////////////////////////////////

PSOManager* PSOManager::GetInstance(){
    if(!instance_){
        instance_ = new PSOManager();
    }

    return instance_;
}

PSOManager::~PSOManager(){}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                     CS用の作成関数
//////////////////////////////////////////////////////////////////////////////////////////////


ID3D12RootSignature* PSOManager::SettingCSRootSignature(){
    // ディスクリプタ範囲を定義（SRV、UAV、CBV）
    CD3DX12_DESCRIPTOR_RANGE ranges[5]{};
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0: inputTexture
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // u0: outputTexture
    ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); // t1: inputDepthTexture
    ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1); // u1: outputDepthTexture
    ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // b0: inputTexture

    //ルートパラメーターの設定
    CD3DX12_ROOT_PARAMETER rootParameters[5]{};
    rootParameters[0].InitAsDescriptorTable(1, &ranges[0]);
    rootParameters[1].InitAsDescriptorTable(1, &ranges[1]);
    rootParameters[2].InitAsDescriptorTable(1, &ranges[2]);
    rootParameters[3].InitAsDescriptorTable(1, &ranges[3]);
    rootParameters[4].InitAsConstantBufferView(0);


    /*------------ Samplerの設定 ------------*/

    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT; // ポイントサンプリング(1箇所だけサンプリングする)
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
    staticSamplers[0].ShaderRegister = 0; // レジスタ番号s0を使う
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // すべてのシェーダー共通で使う


    /*-----------rootSignetureの作成-----------*/

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(_countof(rootParameters), rootParameters);
    rootSignatureDesc.pStaticSamplers = staticSamplers;
    rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);


    ID3DBlob* serializedRootSignature = nullptr;
    ID3DBlob* errorBlob = nullptr;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
    ID3D12RootSignature* rootSignature;
    DxManager::GetInstance()->device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    serializedRootSignature->Release();
    if(errorBlob){
        errorBlob->Release();
    }

    return rootSignature;
}




//////////////////////////////////////////////////////////////////////////////////////////////
//                                      全般PSOの作成関数
//////////////////////////////////////////////////////////////////////////////////////////////

void PSOManager::Create(
    ID3D12RootSignature** pRootSignature, ID3D12PipelineState** pPipelineState,
    PolygonTopology topology, BlendMode blendMode
){
    HRESULT hr;
    ID3DBlob* signatureBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    /*-------------------- RootSignatureの作成 ----------------------*/

    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

    // RootSignatureに関する設定を記述していく
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    /*--------------- ルートパラメーターの設定 ----------------*/

    D3D12_ROOT_PARAMETER rootParameters[4] = {};

    /*---------------------- material ------------------------*/
    D3D12_DESCRIPTOR_RANGE descriptorRangeMaterial[1] = {};
    descriptorRangeMaterial[0].BaseShaderRegister = 0; // t0 から始まる
    descriptorRangeMaterial[0].NumDescriptors = 1; // 1個の SRV を格納
    descriptorRangeMaterial[0].RegisterSpace = 0;
    descriptorRangeMaterial[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV を使う
    descriptorRangeMaterial[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // 自動計算

    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTable を使用
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShader で使う
    rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRangeMaterial; // material のディスクリプタテーブル
    rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeMaterial); // 1つの範囲

    /*---------------------- transform ------------------------*/
    D3D12_DESCRIPTOR_RANGE descriptorRangeTransform[1] = {};
    descriptorRangeTransform[0].BaseShaderRegister = 0; // t1 から始まる
    descriptorRangeTransform[0].NumDescriptors = 1; // 1個の SRV を格納
    descriptorRangeTransform[0].RegisterSpace = 0;
    descriptorRangeTransform[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV を使う
    descriptorRangeTransform[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // 自動計算

    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTable を使用
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShader で使う
    rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeTransform; // transform のディスクリプタテーブル
    rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeTransform); // 1つの範囲

    /*------------------------ texture -----------------------*/
    D3D12_DESCRIPTOR_RANGE descriptorRangeTexture[1] = {};
    descriptorRangeTexture[0].BaseShaderRegister = 1; // t0 から始まる
    descriptorRangeTexture[0].NumDescriptors = 128; // 128個の SRV を格納
    descriptorRangeTexture[0].RegisterSpace = 0;
    descriptorRangeTexture[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV を使う
    descriptorRangeTexture[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // 自動計算

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTable を使用
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShader で使う
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeTexture; // texture も material のテーブルを使う
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeTexture); // material テーブルを利用

    /*---------------------- Lighting ---------------------------*/
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBV を使用
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShader で使う
    rootParameters[3].Descriptor.ShaderRegister = 0; // レジスタ番号 b0 を使う

    //=============================================================================================//

    descriptionRootSignature.pParameters = rootParameters; // ルートパラメーターへのポインタ
    descriptionRootSignature.NumParameters = _countof(rootParameters); // パラメーターの配列数

    /*------------ Samplerの設定 ------------*/

    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT; // ポイントサンプリング(1箇所だけサンプリングする)
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
    staticSamplers[0].ShaderRegister = 0; // レジスタ番号s0を使う
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // すべてのシェーダー共通で使う
    descriptionRootSignature.pStaticSamplers = staticSamplers;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

    /*--------------------------------------*/

    // バイナリを生成
    hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    // 失敗した場合
    if(FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    // RootSignatureの作成
    hr = DxManager::GetInstance()->device->CreateRootSignature(
        0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(pRootSignature)
    );
    assert(SUCCEEDED(hr));

    /*--------------------------------- InputLayoutの設定 ----------------------------------*/

    /*
                     ~ InputLayoutとは ~
    CPUからVertexShaderに渡すデータがどのようなものなのか指定するもの
           VertexShaderInputにあるメンバ変数の数だけ設定する
    */
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[4]{};

    // 頂点座標
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].InputSlot = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    // テクスチャ座標
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].InputSlot = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    // 法線ベクトル
    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].InputSlot = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    // 要素のオフセット
    inputElementDescs[3].SemanticName = "INDEX_OFFSET";
    inputElementDescs[3].SemanticIndex = 0;
    inputElementDescs[3].InputSlot = 1;
    inputElementDescs[3].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;//インスタンスごと
    inputElementDescs[3].Format = DXGI_FORMAT_R32_SINT;
    inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    // 上で作成したデータ群をひとつの変数に入れる
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    /*--------------------------------- BlendStateの設定 ----------------------------------*/

    D3D12_BLEND_DESC blendDesc{};
    // すべての色を書き込むよう設定
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    //// ブレンドステートの設定 (アルファブレンド)
    //blendDesc.AlphaToCoverageEnable = FALSE;
    //blendDesc.IndependentBlendEnable = FALSE;

    //// RenderTarget[0] のアルファブレンド設定
    blendDesc.RenderTarget[0].BlendEnable = TRUE;  // ブレンドを有効にする
    //blendDesc.RenderTarget[0].LogicOpEnable = FALSE;

    // ブレンドモードに応じた設定
    if(blendMode == BlendMode::NORMAL){
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

    } else if(blendMode == BlendMode::ADD){
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

    } else if(blendMode == BlendMode::SUBTRACT){
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

    } else if(blendMode == BlendMode::MULTIPLY){
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;

    } else if(blendMode == BlendMode::SCREEN){
        blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
        blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

    } else{
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }

    // アルファチャンネルのブレンド
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;  // アルファ値はそのまま
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;  // アルファの加算
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;  // アルファ値に影響しない

    //blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    //blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    /*--------------------------------- RasterizerStateの設定 ----------------------------------*/

    D3D12_RASTERIZER_DESC rasterizerDesc{};
    if(topology == PolygonTopology::TRIANGLE){
        rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;// 裏面を表示しない
    } else{
        rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;// 表示
    }
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;// 三角形の中を塗りつぶす
    rasterizerDesc.MultisampleEnable = FALSE; // アンチエイリアシング無効化
    rasterizerDesc.AntialiasedLineEnable = FALSE; // ラインアンチエイリアシング無効化

    /*--------------------------- DepthStencilStateの作成 ---------------------------*/

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;// Depth機能有効化

    //if(blendMode == BlendMode::ADD){
    //    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;// 書き込みしない
    //} else{
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;// 書き込みする
    //}

    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;// 近いものを優先して描画

    /*--------------------------------- PSOの作成 -----------------------------------*/

    // RootSignature
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = *pRootSignature;
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; // InputLayout
    graphicsPipelineStateDesc.VS = { DxManager::GetInstance()->vertexShaderBlob->GetBufferPointer(),
    DxManager::GetInstance()->vertexShaderBlob->GetBufferSize()}; // VertexShader
    graphicsPipelineStateDesc.PS = { DxManager::GetInstance()->pixelShaderBlob->GetBufferPointer(),
    DxManager::GetInstance()->pixelShaderBlob->GetBufferSize() };// PixelShader
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;// DepsStencilState
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    graphicsPipelineStateDesc.BlendState = blendDesc; // BlendState
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState
    // 書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    // 利用するトポロジ (形状)のタイプ。
    if(topology == PolygonTopology::TRIANGLE){
        graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    } else{
        graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    }
    // どのように画面に色を打ち込むかの設定 (気にしなくて良い)
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    // 実際に生成
    hr = DxManager::GetInstance()->device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(pPipelineState));


    // 解放
    signatureBlob->Release();;
    if(errorBlob){
        errorBlob->Release();
    }

    assert(SUCCEEDED(hr));
}
