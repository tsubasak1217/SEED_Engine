#include "ScanLine.h"
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <Environment/Environment.h>
#include <SEED/Source/SEED.h>

ScanLine::ScanLine() : IPostProcess(){
}

void ScanLine::Initialize(){
    // パイプラインの作成
    PSOManager::CreatePipelines("ScanLine.pip");
}


// 更新処理
void ScanLine::Update(){
    time_ += ClockManager::DeltaTime();
}


// ブラーの適用
void ScanLine::Apply(){

    // バインド
    PSOManager::SetBindInfo("ScanLine.pip", "inputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
    PSOManager::SetBindInfo("ScanLine.pip", "outputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
    PSOManager::SetBindInfo("ScanLine.pip", "time", &time_);
    PSOManager::SetBindInfo("ScanLine.pip", "stripeFrequency", &stripeFrequency_);
    PSOManager::SetBindInfo("ScanLine.pip", "scanLineStrength", &scanLineStrength_);
    PSOManager::SetBindInfo("ScanLine.pip", "textureWidth", &kWindowSizeX);
    PSOManager::SetBindInfo("ScanLine.pip", "textureHeight", &kWindowSizeY);

    // ディスパッチ
    Dispatch("ScanLine.pip");
}

// 解放処理
void ScanLine::Release(){

}

// 編集用の関数
void ScanLine::Edit(){
#ifdef _DEBUG
    static std::string label;
    label = "ScanLine_" + std::to_string(id_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        {
            IPostProcess::Edit();
            label = "スキャンライン密度##" + std::to_string(id_);
            ImGui::SliderFloat(label.c_str(), & stripeFrequency_, 0.0f, 1.0f);

            label = "スキャンラインの強さ##" + std::to_string(id_);
            ImGui::SliderFloat(label.c_str(), &scanLineStrength_, 0.0f, 1.0f);
        }
        ImGui::Unindent();
    }
#endif // _DEBUG
}
