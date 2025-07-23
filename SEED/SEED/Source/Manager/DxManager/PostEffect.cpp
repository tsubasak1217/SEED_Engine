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
    PSOManager::SetBindInfo(name, "resolutionRate", &resolutionRate_);
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

    //================ ポストエフェクト用のResourceの初期化 =================//
    for(int i = 0; i < 2; i++){
        // Resourceの作成
        postEffectTextureResource[i].resource = InitializeTextureResource(
            DxManager::GetInstance()->GetDevice(),
            SEED::GetInstance()->kClientWidth_, SEED::GetInstance()->kClientHeight_,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            STATE_UNORDERED_ACCESS
        );

        std::wstring name = L"postEffectResource_" + std::to_wstring(i);
        postEffectTextureResource[i].resource->SetName(name.c_str());
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

    // ポストエフェクトの結果を格納するResource
    {
        // Resourceの作成
        postEffectResultResource.resource = InitializeTextureResource(
            DxManager::GetInstance()->GetDevice(),
            SEED::GetInstance()->kClientWidth_, SEED::GetInstance()->kClientHeight_,
            DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
            STATE_SHADER_RESOURCE
        );

        std::wstring name = L"postEffectResult";
        postEffectResultResource.resource->SetName(name.c_str());
        postEffectResultResource.InitState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // SRVのDesc設定
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = postEffectResultResource.resource->GetDesc().Format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
        srvDesc.Texture2D.MipLevels = 1;

        // UAVのDesc設定
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = postEffectResultResource.resource->GetDesc().Format;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

        // Viewの生成
        ViewManager::CreateView(VIEW_TYPE::SRV, postEffectResultResource.resource.Get(), &srvDesc, "postEffectResult");
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
// ポストエフェクトのディスパッチ
////////////////////////////////////////////////////////////////////////////////////////
void PostEffect::Dispatch(std::string pipelineName,int32_t gridX,int32_t gridY){
    // ポストエフェクトの出力テクスチャをバインド
    std::string resourceName = currentBufferIndex_ == 0 ? "postEffect_1_UAV" : "postEffect_0_UAV";
    PSOManager::SetBindInfo(pipelineName, "outputTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, resourceName));

    // 入力テクスチャをバインド
    resourceName = currentBufferIndex_ == 0 ? "postEffect_0" : "postEffect_1";
    PSOManager::SetBindInfo(pipelineName, "inputTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, resourceName));

    // PSOの切り替え
    auto* pso = PSOManager::GetPSO_Compute(pipelineName);
    auto& commandList = DxManager::GetInstance()->commandList;
    commandList->SetComputeRootSignature(pso->rootSignature.get()->rootSignature.Get());
    commandList->SetPipelineState(pso->pipeline.get()->pipeline_.Get());

    // 解像度を更新
    resolutionRate_ = DxManager::GetInstance()->resolutionRate_;

    // リソースのバインド
    pso->rootSignature->BindAll(commandList.Get(), true);

    // 画面をグリッドに分割して、縦横それぞれのGroup数を計算
    UINT dispatchX = (SEED::GetInstance()->kClientWidth_ + (gridX - 1)) / gridX;
    UINT dispatchY = (SEED::GetInstance()->kClientHeight_ + (gridY - 1)) / gridY;

    // 実行
    commandList->Dispatch(dispatchX, dispatchY, 1);

    // バッファインデックスを更新
    currentBufferIndex_ = (currentBufferIndex_ + 1) % 2;

    // GPUを待つ
    DxManager::instance_->WaitForGPU();

    // リソースの状態を更新
    StartTransition();
}


//////////////////////////////////////////////////////////////////////////////////////
// オフスクリーンの内容をポストエフェクト用のテクスチャにコピー
//////////////////////////////////////////////////////////////////////////////////////
void PostEffect::CopyOffScreen(){

    // コピー状態を設定
    auto& src = DxManager::instance_->offScreenResources[SEED::GetMainCameraName()];
    auto& dst = postEffectTextureResource[currentBufferIndex_];
    src.TransitionState(D3D12_RESOURCE_STATE_COPY_SOURCE);
    dst.TransitionState(D3D12_RESOURCE_STATE_COPY_DEST);

    // コピーを行う
    auto& commandList = DxManager::GetInstance()->commandList;
    commandList->CopyResource(
        postEffectTextureResource[currentBufferIndex_].resource.Get(),
        DxManager::instance_->offScreenResources[SEED::GetMainCameraName()].resource.Get()
    );

    // コピー後の状態を設定
    src.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    dst.TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
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
// 有効なポストエフェクトを適用する処理
////////////////////////////////////////////////////////////////////////////////////////
void PostEffect::PostProcess(){

#ifdef _DEBUG
    ImFunc::CustomBegin("ポストエフェクト", MoveOnly_TitleBar);
    ImFunc::BitMask("有効ポストエフェクト", postEffectBit_, {"グレースケール", "被写界深度"});
    ImGui::End();
#endif // _DEBUG

    // ポストエフェクトの開始
    CopyOffScreen();
    StartTransition();

    // グレースケール処理
    if((int)postEffectBit_ & (int)PostEffectBit::Grayscale){
        Grayscale();
    }

    // 被写界深度処理
    if((int)postEffectBit_ & (int)PostEffectBit::DoF){
        DoF();
    }

    // ポストエフェクトの終了
    EndTransition();
}

////////////////////////////////////////////////////////////////////////////////////////
// グレースケール
////////////////////////////////////////////////////////////////////////////////////////


void PostEffect::Grayscale(){
    // ディスパッチ
    Dispatch("GrayScale.pip");
}

////////////////////////////////////////////////////////////////////////////////////////
// 被写界深度の処理
////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::DoF(){

    // メインカメラに応じて変わる情報をバインド
    std::string resourceName = DxManager::instance_->depthStencilNames[SEED::GetMainCameraName()];
    PSOManager::SetBindInfo("DoFPipeline.pip", "inputDepthTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV,resourceName));

    // ディスパッチ
    Dispatch("DoFPipeline.pip", 16, 16);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ポストエフェクト開始時の遷移
////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::StartTransition(){

    // ポストエフェクトの結果格納用Transition
    postEffectTextureResource[currentBufferIndex_].TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    postEffectTextureResource[(currentBufferIndex_ + 1) % 2].TransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
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

    // 結果格納用のバッファに結果をコピーする(SRGBで保持用)
    {
        // コピー状態を設定
        auto& src = postEffectTextureResource[currentBufferIndex_];
        auto& dst = postEffectResultResource;
        src.TransitionState(D3D12_RESOURCE_STATE_COPY_SOURCE);
        dst.TransitionState(D3D12_RESOURCE_STATE_COPY_DEST);

        // コピーを行う
        auto& commandList = DxManager::GetInstance()->commandList;
        commandList->CopyResource(
            postEffectResultResource.resource.Get(),
            postEffectTextureResource[currentBufferIndex_].resource.Get()
        );

        // コピー後の状態を設定
        src.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        dst.TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}

