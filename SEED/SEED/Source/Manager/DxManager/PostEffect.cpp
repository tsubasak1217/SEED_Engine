// local
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Structs/CS_Buffers.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <SEED/Source/Basic/PostProcess/IPostProcess.h>

// external
#include <cmath>

// postProcess
#include <SEED/Source/Basic/PostProcess/GaussianFilter/GaussianFilter.h>
#include <SEED/Source/Basic/PostProcess/DoF/DoF.h>
#include <SEED/Source/Basic/PostProcess/ScanLine/ScanLine.h>
#include <SEED/Source/Basic/PostProcess/RGBShift/RGBShift.h>
#include <SEED/Source/Basic/PostProcess/GrayScale/GrayScale.h>
#include <SEED/Source/Basic/PostProcess/Vignette/Vignette.h>
#include <SEED/Source/Basic/PostProcess/Fog/Fog.h>

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
    CreateResources();
}


//////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////
void PostEffect::Update(){
    for(auto& postProcess : instance_->postProcesses_){
        if(postProcess->GetIsActive()){
            postProcess->Update();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// ファイルからポストエフェクトに必要なパイプラインを読み込み作成
//////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::InitPSO(){
    PSOManager::CreatePipelines("DoFPipeline.pip");// 被写界深度パイプライン
    PSOManager::CreatePipelines("GrayScale.pip");// グレースケールパイプライン
    PSOManager::CreatePipelines("Vignette.pip");// ビネットパイプライン
    PSOManager::CreatePipelines("RGBShift.pip");// RGBシフトパイプライン
    PSOManager::CreatePipelines("ScanLine.pip");// スキャンラインパイプライン
}


/////////////////////////////////////////////////////////////////////////////////////////////
// リソースを作成する
/////////////////////////////////////////////////////////////////////////////////////////////
void PostEffect::CreateResources(){

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
    for(auto& postProcess : postProcesses_){
        postProcess->Release();
    }

    for(int i = 0; i < 2; i++){
        if(postEffectTextureResource[i].resource){
            postEffectTextureResource[i].resource->Release();
        }
    }

    if(postEffectResultResource.resource){
        postEffectResultResource.resource->Release();
    }
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

    // ポストエフェクトの開始
    CopyOffScreen();
    StartTransition();

    // 有効なポストエフェクトを適用
    for(auto& postProcess : postProcesses_){
        if(postProcess->GetIsActive()){
            postProcess->Apply();
        }
    }

    // ポストエフェクトの終了
    EndTransition();
}


////////////////////////////////////////////////////////////////////////////////////////////
// ポストエフェクト開始時の遷移
////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::StartTransition(){

    // ポストエフェクトの結果格納用Transition
    postEffectTextureResource[currentBufferIndex_].TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    postEffectTextureResource[(currentBufferIndex_ + 1) % 2].TransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

////////////////////////////////////////////////////////////////////////////////////////
// ポストエフェクト終了時の遷移
////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::EndTransition(){
    // ぼかしテクスチャのTransition
    postEffectTextureResource[currentBufferIndex_].TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

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


void PostEffect::Edit(){
#ifdef _DEBUG
    ImFunc::CustomBegin("ポストエフェクト", MoveOnly_TitleBar);
    {
        // 新しいポストエフェクトの追加
        if(ImGui::CollapsingHeader("新しいポストエフェクトの追加")){
            ImGui::Indent();
            {
                if(ImGui::Button("グレースケール")){
                    postProcesses_.emplace_back(std::make_unique<GrayScale>());
                    postProcesses_.back()->Initialize();
                }
                if(ImGui::Button("ガウスぼかし")){
                    postProcesses_.emplace_back(std::make_unique<GaussianFilter>());
                    postProcesses_.back()->Initialize();
                }
                if(ImGui::Button("被写界深度")){
                    postProcesses_.emplace_back(std::make_unique<DoF>());
                    postProcesses_.back()->Initialize();
                }
                if(ImGui::Button("ビネット")){
                    postProcesses_.emplace_back(std::make_unique<Vignette>());
                    postProcesses_.back()->Initialize();
                }
                if(ImGui::Button("RGBシフト")){
                    postProcesses_.emplace_back(std::make_unique<RGBShift>());
                    postProcesses_.back()->Initialize();
                }
                if(ImGui::Button("スキャンライン")){
                    postProcesses_.emplace_back(std::make_unique<ScanLine>());
                    postProcesses_.back()->Initialize();
                }
                if(ImGui::Button("フォグ")){
                    postProcesses_.emplace_back(std::make_unique<Fog>());
                    postProcesses_.back()->Initialize();
                }


            }ImGui::Unindent();
        }


        for(auto& postProcess : postProcesses_){
            postProcess->Edit();
        }

    }ImGui::End();
#endif // _DEBUG
}