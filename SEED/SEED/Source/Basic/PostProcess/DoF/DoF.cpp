#include "DoF.h"
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <Environment/Environment.h>
#include <SEED/Source/SEED.h>

DoF::DoF() : IPostProcess(){
}

void DoF::Initialize(){
    // パイプラインの作成
    PSOManager::CreatePipelines("DoF.pip");
}


// 特になし
void DoF::Update(){
    resolutionRate_ = SEED::GetResolutionRate();
    focusDepth_ = focusDistance_ / (SEED::GetMainCamera()->GetZFar() - SEED::GetMainCamera()->GetZNear());
    focusDepth_ = std::clamp(focusDepth_, 0.0f, 1.0f);
}


// ブラーの適用
void DoF::Apply(){

    // バインド
    PSOManager::SetBindInfo("DoF.pip", "inputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
    PSOManager::SetBindInfo("DoF.pip", "outputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
    PSOManager::SetBindInfo("DoF.pip", "inputDepthTexture", 
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurDepthBufferName()));
    PSOManager::SetBindInfo("DoF.pip", "resolutionRate", &resolutionRate_);
    PSOManager::SetBindInfo("DoF.pip", "focusDepth", &focusDepth_);
    PSOManager::SetBindInfo("DoF.pip", "focusRange", &focusRange_);

    // ディスパッチ
    Dispatch("DoF.pip");
}

// 解放処理
void DoF::Release(){
    // リソースの解放

}

// 編集用の関数
void DoF::Edit(){
#ifdef _DEBUG
    static std::string label;
    label = "DoF_" + std::to_string(id_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        {
            IPostProcess::Edit();
            label = "焦点距離##" + std::to_string(id_);
            ImGui::SliderFloat(label.c_str(), &focusDistance_, SEED::GetMainCamera()->GetZNear(), SEED::GetMainCamera()->GetZFar(), "%.2f");
            
            label = "フォーカス範囲##" + std::to_string(id_);
            ImGui::SliderFloat(label.c_str(), &focusRange_, 0.0f, 1.0f, "%.2f");
        }
        ImGui::Unindent();
    }
#endif // _DEBUG
}
