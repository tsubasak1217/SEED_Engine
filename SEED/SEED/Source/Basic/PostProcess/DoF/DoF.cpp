#include "DoF.h"
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <SEED/Source/SEED.h>

DoF::DoF() : IPostProcess(){
}

////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
////////////////////////////////////////////////////////////////////////////////////////////
void DoF::Initialize(){
    // パイプラインの作成
    PSOManager::CreatePipelines("DownSample.pip");
    PSOManager::CreatePipelines("GaussianFilter/GaussianFilterX.pip");
    PSOManager::CreatePipelines("GaussianFilter/GaussianFilterY.pip");
    PSOManager::CreatePipelines("DoF.pip");

    // リソースの作成
    for(int i = 0; i < 2; i++){
        
        if(halfTexture_[i].resource){
            continue; // 既にリソースを作成している場合はスキップ
        }
        
        // Resourceの作成
        halfTexture_[i].resource = InitializeTextureResource(
            DxManager::GetInstance()->GetDevice(),
            downSampledWidth_, downSampledHeight_,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            STATE_UNORDERED_ACCESS
        );

        std::wstring name = L"halfResource_" + std::to_wstring(i);
        halfTexture_[i].resource->SetName(name.c_str());
        halfTexture_[i].InitState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // SRVのDesc設定
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = halfTexture_[i].resource->GetDesc().Format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
        srvDesc.Texture2D.MipLevels = 1;

        // UAVのDesc設定
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = halfTexture_[i].resource->GetDesc().Format;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

        // Viewの生成
        ViewManager::CreateView(VIEW_TYPE::SRV, halfTexture_[i].resource.Get(), &srvDesc, "DoF_HalfTex_" + std::to_string(i));
        ViewManager::CreateView(VIEW_TYPE::UAV, halfTexture_[i].resource.Get(), &uavDesc, "DoF_HalfTex_" + std::to_string(i) + "_UAV");
    }

    // バッファの作成
    blurParamsBuffer_.bufferResource.resource = CreateBufferResource(GetDevice(), sizeof(BlurParams));
    // Mapする
    blurParamsBuffer_.Map();
    // パラメータの初期化
    blurParamsBuffer_.data->texelSize = {
        1.0f / downSampledWidth_,
        1.0f / downSampledHeight_
    };

    blurParamsBuffer_.data->blurRadius = 5; // ブラー半径の初期値
}

////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////////////////////////////
void DoF::Update(){
    focusDepth_ = focusDistance_ / (SEED::GetMainCamera()->GetZFar() - SEED::GetMainCamera()->GetZNear());
    focusDepth_ = std::clamp(focusDepth_, 0.0f, 1.0f);

    // 
    downSampledHeight_ = int(float(kWindowSizeY) * resolutionRate_);
    downSampledWidth_ = int(float(kWindowSizeX) * resolutionRate_);
}


////////////////////////////////////////////////////////////////////////////////////////////
// エフェクトの適用
////////////////////////////////////////////////////////////////////////////////////////////
void DoF::Apply(){

    // リソースの状態を変更
    halfTexture_[currentBufferIndex_].TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    halfTexture_[(currentBufferIndex_ + 1) % 2].TransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


    {// ダウンサンプリング
        PSOManager::SetBindInfo("DownSample.pip", "inputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
        PSOManager::SetBindInfo("DownSample.pip", "outputHalfTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, currentBufferIndex_ == 0 ? "DoF_HalfTex_1_UAV" : "DoF_HalfTex_0_UAV"));
        PSOManager::SetBindInfo("DownSample.pip", "textureWidth", &kWindowSizeX);
        PSOManager::SetBindInfo("DownSample.pip", "textureHeight", &kWindowSizeY);
        PSOManager::SetBindInfo("DownSample.pip", "resolutionRate", &resolutionRate_);

        // ディスパッチ
        Dispatch("DownSample.pip", 16,16, kWindowSizeX, kWindowSizeY,false);
        SwapBuffer();
    }

    {// X方向ブラー
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterX.pip", "inputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, currentBufferIndex_ == 0 ? "DoF_HalfTex_0" : "DoF_HalfTex_1"));
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterX.pip", "outputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, currentBufferIndex_ == 0 ? "DoF_HalfTex_1_UAV" : "DoF_HalfTex_0_UAV"));
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterX.pip", "BlurParams",
            blurParamsBuffer_.bufferResource.resource->GetGPUVirtualAddress());

        // X方向のブラー適用
        Dispatch("GaussianFilter/GaussianFilterX.pip", 16, 16, downSampledWidth_, downSampledHeight_, false);
        SwapBuffer();
    }

    {// Y方向ブラー
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterY.pip", "inputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, currentBufferIndex_ == 0 ? "DoF_HalfTex_0" : "DoF_HalfTex_1"));
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterY.pip", "outputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, currentBufferIndex_ == 0 ? "DoF_HalfTex_1_UAV" : "DoF_HalfTex_0_UAV"));
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterY.pip", "BlurParams",
            blurParamsBuffer_.bufferResource.resource->GetGPUVirtualAddress());

        // X方向のブラー適用
        Dispatch("GaussianFilter/GaussianFilterY.pip", 16, 16, downSampledWidth_, downSampledHeight_, false);
        SwapBuffer();
    }

    {// 被写界深度ブレンド
        PSOManager::SetBindInfo("DoF.pip", "inputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
        PSOManager::SetBindInfo("DoF.pip", "inputBlurTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, currentBufferIndex_ == 0 ? "DoF_HalfTex_0" : "DoF_HalfTex_1"));
        PSOManager::SetBindInfo("DoF.pip", "inputDepthTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurDepthBufferName()));
        PSOManager::SetBindInfo("DoF.pip", "outputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
        PSOManager::SetBindInfo("DoF.pip", "resolutionRate", &resolutionRate_);
        PSOManager::SetBindInfo("DoF.pip", "focusDepth", &focusDepth_);
        PSOManager::SetBindInfo("DoF.pip", "focusRange", &focusRange_);
        PSOManager::SetBindInfo("DoF.pip", "textureWidth", &kWindowSizeX);
        PSOManager::SetBindInfo("DoF.pip", "textureHeight", &kWindowSizeY);

        // ディスパッチ
        Dispatch("DoF.pip");
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
// 解放処理
////////////////////////////////////////////////////////////////////////////////////////////
void DoF::Release(){
    // リソースの解放
    if(blurParamsBuffer_.bufferResource.resource){
        blurParamsBuffer_.bufferResource.resource->Release();
    }
    for(int i = 0; i < 2; i++){
        if(halfTexture_[i].resource){
            halfTexture_[i].resource->Release();
            halfTexture_[i].resource = nullptr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// バッファのスワップ
////////////////////////////////////////////////////////////////////////////////////////////
void DoF::SwapBuffer(){
    currentBufferIndex_ = (currentBufferIndex_ + 1) % 2;

    // バッファの状態を変更
    halfTexture_[currentBufferIndex_].TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    halfTexture_[(currentBufferIndex_ + 1) % 2].TransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}


////////////////////////////////////////////////////////////////////////////////////////////
// 編集用の関数
////////////////////////////////////////////////////////////////////////////////////////////
void DoF::Edit(){
#ifdef _DEBUG
    static std::string label;
    label = "DoF_" + std::to_string(id_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        {
            IPostProcess::Edit();

            label = "ダウンサンプリングの解像度##" + std::to_string(id_);
            ImGui::SliderFloat(label.c_str(), &resolutionRate_, 0.1f, 1.0f, "%.2f");

            label = "ぼかし半径##" + std::to_string(id_);
            ImGui::SliderInt(label.c_str(), &blurParamsBuffer_.data->blurRadius, 1, 30);

            label = "焦点距離##" + std::to_string(id_);
            ImGui::SliderFloat(label.c_str(), &focusDistance_, SEED::GetMainCamera()->GetZNear(), SEED::GetMainCamera()->GetZFar(), "%.2f");
            
            label = "フォーカス範囲##" + std::to_string(id_);
            ImGui::SliderFloat(label.c_str(), &focusRange_, 0.0f, 1.0f, "%.2f");
        }
        ImGui::Unindent();
    }
#endif // _DEBUG
}


////////////////////////////////////////////////////////////////////////////////////////////
// 入出力
////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json DoF::ToJson(){
    nlohmann::json j;

    j["type"] = "DoF";
    j["resolutionRate"] = resolutionRate_;
    j["focusDistance"] = focusDistance_;
    j["focusDepth"] = focusDepth_;
    j["focusRange"] = focusRange_;
    j["blurRadius"] = blurParamsBuffer_.data->blurRadius;

    return j;
}

void DoF::FromJson(const nlohmann::json& json){
    if(json.contains("resolutionRate")){
        resolutionRate_ = json["resolutionRate"].get<float>();
    }
    if(json.contains("focusDistance")){
        focusDistance_ = json["focusDistance"].get<float>();
    }
    if(json.contains("focusDepth")){
        focusDepth_ = json["focusDepth"].get<float>();
    }
    if(json.contains("focusRange")){
        focusRange_ = json["focusRange"].get<float>();
    }
    if(json.contains("blurRadius")){
        blurParamsBuffer_.data->blurRadius = json["blurRadius"].get<int>();
    }
    // バッファの更新
    blurParamsBuffer_.data->texelSize = {
        1.0f / downSampledWidth_,
        1.0f / downSampledHeight_
    };
}
