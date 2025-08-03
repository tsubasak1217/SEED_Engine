#include "GrayScale.h"
#include <SEED/Source/Manager/DxManager/PostEffect.h>
#include <Environment/Environment.h>
#include <SEED/Source/SEED.h>

GrayScale::GrayScale() : IPostProcess(){
}

void GrayScale::Initialize(){
    // パイプラインの作成
    PSOManager::CreatePipelines("GrayScale.pip");
}

// ブラーの適用
void GrayScale::Apply(){

    // バインド
    PSOManager::SetBindInfo("GrayScale.pip", "inputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
    PSOManager::SetBindInfo("GrayScale.pip", "outputTexture",
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
    PSOManager::SetBindInfo("GrayScale.pip", "textureWidth", &kWindowSizeX);
    PSOManager::SetBindInfo("GrayScale.pip", "textureHeight", &kWindowSizeY);

    // ディスパッチ
    Dispatch("GrayScale.pip");
}

// 解放処理
void GrayScale::Release(){

}

// 編集用の関数
void GrayScale::Edit(){
#ifdef _DEBUG
    static std::string label;
    label = "GrayScale_" + std::to_string(id_);
    if(ImGui::CollapsingHeader(label.c_str())){
        ImGui::Indent();
        {
            IPostProcess::Edit();
        }
        ImGui::Unindent();
    }
#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////////////////////////
// 入出力
////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json GrayScale::ToJson(){
    nlohmann::json j;
    j["type"] = "GrayScale";
    return j;
}

void GrayScale::FromJson(const nlohmann::json& json){
    json;
}
