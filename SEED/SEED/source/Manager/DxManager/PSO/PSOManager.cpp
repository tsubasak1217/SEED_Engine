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


ComPtr<ID3D12RootSignature> PSOManager::SettingCSRootSignature(){
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


    ComPtr<ID3DBlob> serializedRootSignature = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
    ComPtr<ID3D12RootSignature> rootSignature;
    DxManager::GetInstance()->device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    serializedRootSignature->Release();
    if(errorBlob){
        errorBlob->Release();
    }

    return rootSignature;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  パイプライン、RuutSignatureのテンプレートパラメータの生成関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PSOManager::GenerateTemplateParameter(
    RootSignature* pRootSignature, Pipeline* pPipeline, PippelineType pypelineType
){

    switch(pypelineType){

        /*==================================================================================================================*/
    case PippelineType::Normal:// 通常のパイプライン
        /*==================================================================================================================*/

        /*---------------- RootParameter ------------------*/
        // Camera(PS_b0)
        pRootSignature->AddParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 0);
        // material(PS_t0,1個)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        // transform(VS_t0,1個)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);
        // DirectionalLights(PS_t1)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, D3D12_SHADER_VISIBILITY_PIXEL);
        // PointLights(PS_t2)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, D3D12_SHADER_VISIBILITY_PIXEL);
        // PointLights(PS_t3)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, D3D12_SHADER_VISIBILITY_PIXEL);
        // DirectionalLight数(PS_b1)
        pRootSignature->AddParameter(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, D3D12_SHADER_VISIBILITY_PIXEL, 1, 0, 1);
        // PointLight数(PS_b2)
        pRootSignature->AddParameter(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, D3D12_SHADER_VISIBILITY_PIXEL, 2, 0, 1);
        // SpotLight数(PS_b3)
        pRootSignature->AddParameter(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, D3D12_SHADER_VISIBILITY_PIXEL, 3, 0, 1);
        // texture(PS_t3,128個)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 128, 4, D3D12_SHADER_VISIBILITY_PIXEL);

        /*------------------ InputLayout ------------------*/
        // VBV_0
        pPipeline->AddInputElementDesc("POSITION", 0, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
        pPipeline->AddInputElementDesc("TEXCOORD", 0, 0, DXGI_FORMAT_R32G32_FLOAT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
        pPipeline->AddInputElementDesc("NORMAL", 0, 0, DXGI_FORMAT_R32G32B32_FLOAT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
        // VBV_1
        pPipeline->AddInputElementDesc("INDEX_OFFSET", 0, 1, DXGI_FORMAT_R32_SINT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA);
        pPipeline->AddInputElementDesc("MESH_OFFSET", 0, 1, DXGI_FORMAT_R32_SINT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA);
        pPipeline->AddInputElementDesc("JOINT_INDEX_OFFSET", 0, 1, DXGI_FORMAT_R32_SINT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA);
        pPipeline->AddInputElementDesc("JOINT_INTERVAL", 0, 1, DXGI_FORMAT_R32_SINT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA);
        pPipeline->AddInputElementDesc("INTERVAL", 0, 1, DXGI_FORMAT_R32_SINT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA);

        /*------------------- Shader -----------------------*/
        pPipeline->pVsBlob_ = DxManager::GetInstance()->vsBlobs["commonVS"].Get();
        pPipeline->pPsVlob_ = DxManager::GetInstance()->psBlobs["commonPS"].Get();

        break;

        /*================================================================================================================*/
    case PippelineType::Skinning:// スキニング用のパイプライン
        /*================================================================================================================*/

        /*---------------- RootParameter ------------------*/
        // Camera(PS_b0)
        pRootSignature->AddParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, D3D12_SHADER_VISIBILITY_PIXEL, 0);
        // material(PS_t0,1個)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        // transform(VS_t0,1個)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);
        // DirectionalLights(PS_t1)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, D3D12_SHADER_VISIBILITY_PIXEL);
        // PointLights(PS_t2)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, D3D12_SHADER_VISIBILITY_PIXEL);
        // SpotLights(PS_t3)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, D3D12_SHADER_VISIBILITY_PIXEL);
        // DirectionalLight数(PS_b1)
        pRootSignature->AddParameter(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, D3D12_SHADER_VISIBILITY_PIXEL, 1, 0, 1);
        // PointLight数(PS_b2)
        pRootSignature->AddParameter(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, D3D12_SHADER_VISIBILITY_PIXEL, 2, 0, 1);
        // SpotLight数(PS_b3)
        pRootSignature->AddParameter(D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, D3D12_SHADER_VISIBILITY_PIXEL, 3, 0, 1);
        // texture(PS_t3,128個)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 128, 4, D3D12_SHADER_VISIBILITY_PIXEL);
        // MatrixPalette(VS_t1)
        pRootSignature->AddDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, D3D12_SHADER_VISIBILITY_VERTEX);

        /*------------------ InputLayout ------------------*/
        // VBV_0
        pPipeline->AddInputElementDesc("POSITION", 0, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
        pPipeline->AddInputElementDesc("TEXCOORD", 0, 0, DXGI_FORMAT_R32G32_FLOAT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
        pPipeline->AddInputElementDesc("NORMAL", 0, 0, DXGI_FORMAT_R32G32B32_FLOAT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
        // VBV_1
        pPipeline->AddInputElementDesc("INDEX_OFFSET", 0, 1, DXGI_FORMAT_R32_SINT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA);
        pPipeline->AddInputElementDesc("MESH_OFFSET", 0, 1, DXGI_FORMAT_R32_SINT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA);
        pPipeline->AddInputElementDesc("JOINT_INDEX_OFFSET", 0, 1, DXGI_FORMAT_R32_SINT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA);
        pPipeline->AddInputElementDesc("JOINT_INTERVAL", 0, 1, DXGI_FORMAT_R32_SINT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA);
        pPipeline->AddInputElementDesc("INTERVAL", 0, 1, DXGI_FORMAT_R32_SINT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA);
        // VBV_2
        pPipeline->AddInputElementDesc("WEIGHT", 0, 2, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
        pPipeline->AddInputElementDesc("JOINT_INDEX", 0, 2, DXGI_FORMAT_R32G32B32A32_SINT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);

        /*------------------- Shader -----------------------*/
        pPipeline->pVsBlob_ = DxManager::GetInstance()->vsBlobs["skinningVS"].Get();
        pPipeline->pPsVlob_ = DxManager::GetInstance()->psBlobs["commonPS"].Get();

        break;

    default:
        break;
    }
    if(pypelineType == PippelineType::Normal){



    } else if(pypelineType == PippelineType::Skinning){// スキニング用のパイプライン=======================================


    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  PSOの作成関数
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PSOManager::Create(RootSignature* pRootSignature, Pipeline* pPipeline){
    HRESULT hr;
    ComPtr<ID3DBlob> signatureBlob = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;

    /*-------------------- RootSignatureの作成 ----------------------*/

    // バイナリを生成
    hr = D3D12SerializeRootSignature(&pRootSignature->desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    // 失敗した場合
    if(FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    // RootSignatureの作成
    hr = DxManager::GetInstance()->device->CreateRootSignature(
        0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(pRootSignature->rootSignature.GetAddressOf())
    );

    // 失敗したらアサート
    assert(SUCCEEDED(hr));

    /*------------------------- PSOの作成 ---------------------------*/

    // RootSignature
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = *pRootSignature->rootSignature.GetAddressOf();
    // InputLayout
    graphicsPipelineStateDesc.InputLayout = pPipeline->inputLayout_;
    // shader
    graphicsPipelineStateDesc.VS = { pPipeline->pVsBlob_->GetBufferPointer(),
    pPipeline->pVsBlob_->GetBufferSize() }; // VertexShader
    graphicsPipelineStateDesc.PS = { pPipeline->pPsVlob_->GetBufferPointer(),
        pPipeline->pPsVlob_->GetBufferSize() };// PixelShader
    // DepsStencilState
    graphicsPipelineStateDesc.DepthStencilState = pPipeline->depthStencilDesc_;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    // BlendState
    graphicsPipelineStateDesc.BlendState = pPipeline->blendDesc_;
    // RasterizerState
    graphicsPipelineStateDesc.RasterizerState = pPipeline->rasterizerDesc_;
    // 書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    // 利用するトポロジ (形状)のタイプ。
    graphicsPipelineStateDesc.PrimitiveTopologyType = pPipeline->topologyType_;
    // どのように画面に色を打ち込むかの設定 (気にしなくて良い)
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    // 実際に生成
    hr = DxManager::GetInstance()->device->CreateGraphicsPipelineState(
        &graphicsPipelineStateDesc, IID_PPV_ARGS(pPipeline->pipelineState_.GetAddressOf())
    );

    // 失敗したらアサート
    assert(SUCCEEDED(hr));

    /*---------------------------- 解放 ------------------------------*/

    signatureBlob->Release();;
    if(errorBlob){
        errorBlob->Release();
    }

}
