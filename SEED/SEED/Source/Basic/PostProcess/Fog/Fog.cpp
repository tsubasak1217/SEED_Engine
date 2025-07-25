#include "Fog.h"
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <Environment/Environment.h>

Fog::Fog() : IPostProcess(){
}

void Fog::Initialize(){

    // パイプラインの作成
    PSOManager::CreatePipelines("Fog.pip");

    // リソースの作成
    fogParamsBuffer_.bufferResource.resource = CreateBufferResource(GetDevice(), sizeof(FogParams));

    // Mapする
    fogParamsBuffer_.Map();

    // パラメータの初期化
    fogParamsBuffer_.data->fogStrength = 0.5f; // フォグの強さ
    fogParamsBuffer_.data->fogStartDistance = 0.01f; // フォグの開始深度
    fogParamsBuffer_.data->fogColor = { 0.7f, 0.8f, 1.0f,1.0f }; // フォグの色（グレー）

}


// 特になし
void Fog::Update(){
}


// ブラーの適用
void Fog::Apply(){

    // バインド
    PSOManager::SetBindInfo("Fog.pip", "inputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
    PSOManager::SetBindInfo("Fog.pip", "outputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
    PSOManager::SetBindInfo("Fog.pip", "inputDepthTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurDepthBufferName()));
    PSOManager::SetBindInfo("Fog.pip", "FogParams",
        fogParamsBuffer_.bufferResource.resource->GetGPUVirtualAddress());
    // X方向のブラー適用
    Dispatch("Fog.pip", 16, 16);

}

// 解放処理
void Fog::Release(){
    // リソースの解放
    if(fogParamsBuffer_.bufferResource.resource){
        fogParamsBuffer_.bufferResource.resource->Release();
    }
}

// 編集用の関数
void Fog::Edit(){
#ifdef _DEBUG
    static std::string label;
    label = "Fog_" + std::to_string(id_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        {
            IPostProcess::Edit();
            label = "フォグの強さ##" + std::to_string(id_);
            ImGui::SliderFloat(label.c_str(), &fogParamsBuffer_.data->fogStrength, 0.0f, 1.0f);

            label = "フォグの開始深度##" + std::to_string(id_);
            ImGui::SliderFloat(label.c_str(), &fogParamsBuffer_.data->fogStartDistance, 0.0f, 1.0f);

            label = "フォグの色##" + std::to_string(id_);
            ImGui::ColorEdit3(label.c_str(), &fogParamsBuffer_.data->fogColor.x);
        }
        ImGui::Unindent();
    }
#endif // _DEBUG
}
