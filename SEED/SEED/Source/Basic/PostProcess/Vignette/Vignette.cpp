#include "Vignette.h"
#include <SEED/Source/Manager/PostEffectSystem/PostEffectSystem.h>
#include <Environment/Environment.h>
#include <SEED/Source/SEED.h>

namespace SEED{
    Vignette::Vignette() : IPostProcess(){
    }

    void Vignette::Initialize(){
        // パイプラインの作成
        PSOManager::CreatePipelines("Vignette.pip");
    }

    // ブラーの適用
    void Vignette::Apply(){

        // バインド
        PSOManager::SetBindInfo("Vignette.pip", "inputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurSRVBufferName()));
        PSOManager::SetBindInfo("Vignette.pip", "outputTexture",
            ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, GetCurUAVBufferName()));
        PSOManager::SetBindInfo("Vignette.pip", "vignetteStrength", &vignetteStrength_);
        PSOManager::SetBindInfo("Vignette.pip", "textureWidth", &kWindowSizeX);
        PSOManager::SetBindInfo("Vignette.pip", "textureHeight", &kWindowSizeY);

        // ディスパッチ
        Dispatch("Vignette.pip");
    }

    // 解放処理
    void Vignette::Release(){

    }

    // 編集用の関数
    void Vignette::Edit(){
    #ifdef _DEBUG
        static std::string label;
        label = "Vignette_" + std::to_string(id_);
        if(ImGui::CollapsingHeader(label.c_str())){
            ImGui::Indent();
            {
                IPostProcess::Edit();

                label = "ビネットの強さ##" + std::to_string(id_);
                ImGui::SliderFloat(label.c_str(), &vignetteStrength_, 0.0f, 1.0f, "%.2f");
            }
            ImGui::Unindent();
        }
    #endif // _DEBUG
    }


    ////////////////////////////////////////////////////////////////////////////////////////////
    // 入出力
    ////////////////////////////////////////////////////////////////////////////////////////////
    nlohmann::json Vignette::ToJson(){
        nlohmann::json j;

        j["type"] = "Vignette";
        j["vignetteStrength"] = vignetteStrength_;

        return j;
    }

    void Vignette::FromJson(const nlohmann::json& json){
        if(json.contains("vignetteStrength")){
            vignetteStrength_ = json["vignetteStrength"].get<float>();
        }
    }
}