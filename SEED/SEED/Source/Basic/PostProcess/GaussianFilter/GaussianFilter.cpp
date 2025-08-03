#include "GaussianFilter.h"
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <Environment/Environment.h>

GaussianFilter::GaussianFilter() : IPostProcess(){
}

void GaussianFilter::Initialize(){

    static int gaussianFilterIdx = 0;

    // パイプラインの作成
    PSOManager::CreatePipelines("GaussianFilter/GaussianFilterX.pip");// X方向のブラー用パイプライン
    PSOManager::CreatePipelines("GaussianFilter/GaussianFilterY.pip");// Y方向のブラー用パイプライン

    // リソースの作成
    blurParamsBuffer_.bufferResource.resource = CreateBufferResource(GetDevice(),sizeof(BlurParams));

    // Mapする
    blurParamsBuffer_.Map();

    // パラメータの初期化
    blurParamsBuffer_.data->texelSize = {
        1.0f / kWindowSize.x,
        1.0f / kWindowSize.y
    };

    blurParamsBuffer_.data->blurRadius = 3; // ブラー半径の初期値

    gaussianFilterIdx++;
}


// 特になし
void GaussianFilter::Update(){
}


// ブラーの適用
void GaussianFilter::Apply(){

    {
        // バインド
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterX.pip", "inputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterX.pip", "outputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterX.pip", "BlurParams",
            blurParamsBuffer_.bufferResource.resource->GetGPUVirtualAddress());
        // X方向のブラー適用
        Dispatch("GaussianFilter/GaussianFilterX.pip", 16, 16);
    }

    {
        // バインド
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterY.pip", "inputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterY.pip", "outputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
        PSOManager::SetBindInfo("GaussianFilter/GaussianFilterY.pip", "BlurParams",
            blurParamsBuffer_.bufferResource.resource->GetGPUVirtualAddress());

        // Y方向のブラー適用
        Dispatch("GaussianFilter/GaussianFilterY.pip", 16, 16);
    }
}

// 解放処理
void GaussianFilter::Release(){
    // リソースの解放
    if(blurParamsBuffer_.bufferResource.resource){
        blurParamsBuffer_.bufferResource.resource->Release();
    }
}

// 編集用の関数
void GaussianFilter::Edit(){
#ifdef _DEBUG
    static std::string label;
    label = "GaussianFilter_" + std::to_string(id_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        {
            IPostProcess::Edit();
            label = "ブラー半径##" + std::to_string(id_);
            ImGui::SliderInt(label.c_str(), &blurParamsBuffer_.data->blurRadius, 1, 10);
        }
        ImGui::Unindent();
    }
#endif // _DEBUG
}


////////////////////////////////////////////////////////////////////////////////////////////
// 入出力
////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json GaussianFilter::ToJson(){
    nlohmann::json j;

    j["type"] = "GaussianFilter";
    j["blurRadius"] = blurParamsBuffer_.data->blurRadius;

    return j;
}

void GaussianFilter::FromJson(const nlohmann::json& json){
    if(json.contains("blurRadius")){
        blurParamsBuffer_.data->blurRadius = json["blurRadius"].get<int>();
    }
}
