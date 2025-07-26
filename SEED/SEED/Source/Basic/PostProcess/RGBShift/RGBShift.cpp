#include "RGBShift.h"
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <Environment/Environment.h>
#include <SEED/Source/SEED.h>

RGBShift::RGBShift() : IPostProcess(){
}

void RGBShift::Initialize(){
    // パイプラインの作成
    PSOManager::CreatePipelines("RGBShift.pip");
}

// ブラーの適用
void RGBShift::Apply(){

    // バインド
    PSOManager::SetBindInfo("RGBShift.pip", "inputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
    PSOManager::SetBindInfo("RGBShift.pip", "outputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
    PSOManager::SetBindInfo("RGBShift.pip", "gap", &gap_);
    PSOManager::SetBindInfo("RGBShift.pip", "textureWidth", &kWindowSizeX);
    PSOManager::SetBindInfo("RGBShift.pip", "textureHeight", &kWindowSizeY);

    // ディスパッチ
    Dispatch("RGBShift.pip");
}

// 解放処理
void RGBShift::Release(){

}

// 編集用の関数
void RGBShift::Edit(){
#ifdef _DEBUG
    static std::string label;
    label = "RGBShift_" + std::to_string(id_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        {
            IPostProcess::Edit();
            label = "色のずれ幅##" + std::to_string(id_);
            ImGui::SliderInt(label.c_str(), &gap_, 1, 100);
        }
        ImGui::Unindent();
    }
#endif // _DEBUG
}
