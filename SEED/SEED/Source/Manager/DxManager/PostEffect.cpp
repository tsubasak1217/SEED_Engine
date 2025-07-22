// local
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Structs/CS_Buffers.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>

// external
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////
//                          static変数初期化
////////////////////////////////////////////////////////////////////////////////////////

PostEffect* PostEffect::instance_ = nullptr;


//////////////////////////////////////////////////////////////////////////////////////////
//                          コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////////////////////////////////

PostEffect::~PostEffect(){}

////////////////////////////////////////////////////////////////////////////////////////
//                          初期化・終了関数
////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Initialize(){
    // パイプラインの作成
    InitPSO();
    // Resourceの作成
    CreateResources();
    // リソースのマッピング
    MapOnce();
    // バインド情報の設定
    SetBindInfo();
}


//////////////////////////////////////////////////////////////////////////////////////////////
// ファイルからポストエフェクトに必要なパイプラインを読み込み作成
//////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::InitPSO(){

    // 被写界深度パイプライン
    PSOManager::CreatePipelines("DoFPipeline.pip");
    PSOManager::CreatePipelines("GrayScale.pip");
}


//////////////////////////////////////////////////////////////////////////////////////////////
// 必要なリソースをマップする
//////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::MapOnce(){
}


/////////////////////////////////////////////////////////////////////////////////////////////
// リソースのバインド情報を設定する
/////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::SetBindInfo(){

    // DoFPipeline.pip
    std::string name = "DoFPipeline.pip";
    PSOManager::SetBindInfo(name, "outputDepthTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "depth_1_UAV"));
    PSOManager::SetBindInfo(name, "inputDepthTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "depth_0"));
    PSOManager::SetBindInfo(name, "resolutionRate", &resolutionRate_);

    // GrayScale.pip
    name = "GrayScale.pip";
    PSOManager::SetBindInfo(name, "inputTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "offScreen_0"));
    PSOManager::SetBindInfo(name, "outputTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "postEffect_0_UAV"));

}



/////////////////////////////////////////////////////////////////////////////////////////////
// リソースを作成する
/////////////////////////////////////////////////////////////////////////////////////////////
void PostEffect::CreateResources(){

    //========== 深度情報をテクスチャとして表示する用のResource作成 ===========//
    {
        // Resourceの作成
        depthTextureResource.resource;
        depthTextureResource.resource = InitializeTextureResource(
            DxManager::GetInstance()->GetDevice(),
            SEED::GetInstance()->kClientWidth_, SEED::GetInstance()->kClientHeight_,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            STATE_UNORDERED_ACCESS
        );
        depthTextureResource.resource->SetName(L"depthTextureResource");
        depthTextureResource.InitState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // SRVのDesc設定
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
        srvDesc.Texture2D.MipLevels = 1;

        // UAVのDesc設定
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

        // Viewの生成
        ViewManager::CreateView(VIEW_TYPE::SRV, depthTextureResource.resource.Get(), &srvDesc, "depth_1");
        ViewManager::CreateView(VIEW_TYPE::UAV, depthTextureResource.resource.Get(), &uavDesc, "depth_1_UAV");
    }

    //================ ポストエフェクト結果用のResourceの初期化 =================//
    for(int i = 0; i < 2; i++){
        // Resourceの作成
        postEffectTextureResource[i].resource = InitializeTextureResource(
            DxManager::GetInstance()->GetDevice(),
            SEED::GetInstance()->kClientWidth_, SEED::GetInstance()->kClientHeight_,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            STATE_UNORDERED_ACCESS
        );
        postEffectTextureResource[i].resource->SetName(L"postEffectResource");
        postEffectTextureResource[i].InitState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // SRVのDesc設定
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = postEffectTextureResource[i].resource->GetDesc().Format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
        srvDesc.Texture2D.MipLevels = 1;

        // UAVのDesc設定
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = postEffectTextureResource[i].resource->GetDesc().Format;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

        // Viewの生成
        ViewManager::CreateView(VIEW_TYPE::SRV, postEffectTextureResource[i].resource.Get(), &srvDesc, "postEffect_" + std::to_string(i));
        ViewManager::CreateView(VIEW_TYPE::UAV, postEffectTextureResource[i].resource.Get(), &uavDesc, "postEffect_" + std::to_string(i) + "_UAV");
    }
}



////////////////////////////////////////////////////////////////////////////////////////
// リソースの解放
////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Release(){
    // リソースの解放
    for(int i = 0; i < 2; i++){
        if(postEffectTextureResource[i].resource){
            postEffectTextureResource[i].resource->Release();
        }
    }
    if(depthTextureResource.resource){
        depthTextureResource.resource->Release();
    }
}


////////////////////////////////////////////////////////////////////////////////////////
// インスタンスの取得
////////////////////////////////////////////////////////////////////////////////////////

PostEffect* PostEffect::GetInstance(){
    if(instance_ == nullptr){
        instance_ = new PostEffect();
    }
    return instance_;
}


////////////////////////////////////////////////////////////////////////////////////////
// グレースケール
////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Grayscale(){

    // メインカメラのオフスクリーン情報をバインド
    std::string resourceName = DxManager::instance_->offScreenNames[SEED::GetMainCameraName()];
    PSOManager::SetBindInfo("GrayScale.pip", "inputTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, resourceName));

    // ポストエフェクトの出力テクスチャをバインド
    std::string textureName = currentBufferIndex_ == 0 ? "postEffect_0_UAV" : "postEffect_1_UAV";
    PSOManager::SetBindInfo("GrayScale.pip", "outputTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, textureName));

    // PSOの切り替え
    auto* pso = PSOManager::GetPSO_Compute("GrayScale.pip");
    auto& commandList = DxManager::GetInstance()->commandList;
    commandList->SetComputeRootSignature(pso->rootSignature.get()->rootSignature.Get());
    commandList->SetPipelineState(pso->pipeline.get()->pipeline_.Get());

    // 解像度を更新
    resolutionRate_ = DxManager::GetInstance()->resolutionRate_;

    // リソースのバインド
    pso->rootSignature->BindAll(commandList.Get(), true);

    // 画面を16x16のグリッドに分割して、縦横それぞれのGroup数を計算
    UINT dispatchX = (SEED::GetInstance()->kClientWidth_ + 15) / 16;
    UINT dispatchY = (SEED::GetInstance()->kClientHeight_ + 15) / 16;

    // 実行
    commandList->Dispatch(dispatchX, dispatchY, 1);

    // バッファインデックスを更新
    currentBufferIndex_ = (currentBufferIndex_ + 1) % 2;
}

////////////////////////////////////////////////////////////////////////////////////////
// 被写界深度の処理
////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::DoF(){

    // PSOの切り替え
    auto* pso = PSOManager::GetPSO_Compute("DoFPipeline.pip");
    auto& commandList = DxManager::GetInstance()->commandList;
    commandList->SetComputeRootSignature(pso->rootSignature.get()->rootSignature.Get());
    commandList->SetPipelineState(pso->pipeline.get()->pipeline_.Get());

    // 解像度を更新
    resolutionRate_ = DxManager::GetInstance()->resolutionRate_;

    // リソースのバインド
    pso->rootSignature->BindAll(commandList.Get(), true);

    // 画面を16x16のグリッドに分割して、縦横それぞれのGroup数を計算
    UINT dispatchX = (SEED::GetInstance()->kClientWidth_ + 15) / 16;
    UINT dispatchY = (SEED::GetInstance()->kClientHeight_ + 15) / 16;

    // 実行
    commandList->Dispatch(dispatchX, dispatchY, 1);

    // バッファインデックスを更新
    currentBufferIndex_ = (currentBufferIndex_ + 1) % 2;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ポストエフェクト開始時の遷移
////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::StartTransition(){
    // ぼかしテクスチャのTransition
    postEffectTextureResource[0].TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    postEffectTextureResource[1].TransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    // 深度テクスチャのTransition
    depthTextureResource.TransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

////////////////////////////////////////////////////////////////////////////////////////
// ポストエフェクト終了時の遷移
////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::EndTransition(){
    // ぼかしテクスチャのTransition
    postEffectTextureResource[currentBufferIndex_].TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    // 深度テクスチャのTransition
    depthTextureResource.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    // バッファインデックスを初期化
    currentBufferIndex_ = 0;
}


